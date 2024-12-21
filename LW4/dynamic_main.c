#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void *library1;
static void *library2;

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

void changeImpl(const Impl impl) {
    if (impl == FIRST) {
        Pi = (float(*)(int))dlsym(library1, "Pi");
        E = (float(*)(int))dlsym(library1, "E");
    } else {
        Pi = (float(*)(int))dlsym(library2, "Pi");
        E = (float(*)(int))dlsym(library2, "E");
    }
}

int main() {
    library1 = dlopen("./libfirst.so", RTLD_LAZY);
    if (library1 == NULL) {
        fprintf(stderr, "Error with loading library1: %s\n", dlerror());
        exit(-1);
    }

    library2 = dlopen("./libsecond.so", RTLD_LAZY);
    if (library2 == NULL) {
        fprintf(stderr, "Error with loading library2: %s\n", dlerror());
        exit(-1);
    }

    Impl impl = FIRST;
    printf(
        "Usage:\n\t\b-1 - exit\n\t0 - change implementation\n"
        "\t1 - find Pi\n\t2 - find E\n"
    );
    changeImpl(impl);

    while (1) {
        printf("command> ");
        Command command;
        scanf("%d", &command);

        if (command == EXIT) {
            dlclose(library1);
            dlclose(library2);
            return 0;
        }

        if (command == CHANGE) {
            impl =! impl;
            changeImpl(impl);
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