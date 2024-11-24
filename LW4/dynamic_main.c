#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void *library = NULL;

static float (*Pi)(int);
static float (*E)(int);

typedef enum {
    FIRST,
    SECOND
} Impl;

typedef enum {
    EXIT = -1,
    CHANGE = 0,
    PI = 1,
    EXP = 2
} Command;

void load_lib(const Impl impl) {
    if (impl == FIRST) {
        library = dlopen("./libfirst.so", RTLD_LAZY);
    } else {
        library = dlopen("./libsecond.so", RTLD_LAZY);
    }
    if (library == NULL) {
        fprintf(stderr, "Error with loading library\n");
        exit(-1);
    }

    Pi = (float(*)(int))dlsym(library, "Pi");
    E = (float(*)(int))dlsym(library, "E");
}


int main() {
    Impl impl = FIRST;
    printf(
        "Usage:\n\t\b-1 - exit\n\t0 - change implementation\n"
        "\t1 - find Pi\n\t2 - find E\n"
    );
    load_lib(impl);

    while (1) {
        printf("command> ");
        Command command;
        scanf("%d", &command);

        if (command == EXIT) {
            return 0;
        }

        if (command == CHANGE) {
            impl =! impl;
            dlclose(library);
            load_lib(impl);
            printf("Implementation changed to %s\n", impl == FIRST ? "first" : "second");
        } else if (command == PI) {
            int K;
            scanf(" %d", &K);

            const float result = Pi(K);
            printf("%f\n", result);
        } else if (command == EXP) {
            int x;
            scanf(" %d", &x);

            const float result = E(x);
            printf("%f\n", result);
        }
    }
}