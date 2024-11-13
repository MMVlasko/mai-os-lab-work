#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

static void *library = NULL;

static int (*some)(int);
static int (*home)(int);

typedef enum {
    FIRST,
    SECOND
} Impl;

typedef enum {
    EXIT = -1,
    CHANGE = 0,
    _FIRST = 1,
    _SECOND = 2
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

    some = (int(*)(int))dlsym(library, "some");
    home = (int(*)(int))dlsym(library, "home");
}


int main() {
    Impl impl = FIRST;
    printf(
        "Usage:\n\t\b-1 - exit\n\t0 - change implementation\n"
        "\t1 - count primes\n\t2 - rebase a number\n"
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
            printf("Implementation changed to%s\n", impl == FIRST ? "first" : "second");
        } else if (command == _FIRST) {
            int a;
            scanf(" %d", &a);

            int result = some(a);
            printf("%d\n", result);
        } else if (command == _SECOND) {
            int n;
            scanf(" %d", &n);

            int result = home(n);
            printf("%d\n", result);
        }
    }
}