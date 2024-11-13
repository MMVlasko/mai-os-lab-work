#include <stdio.h>

#include <headers.h>

typedef enum {
    EXIT = -1,
    _FIRST = 1,
    _SECOND = 2
} Command;

int main(void) {
    printf(
        "Usage:\n\t\b-1 - exit\n"
        "\t1 - count primes\n\t2 - rebase a number\n"
    );

    while (1) {
        printf("command> ");
        int command;
        scanf("%d", &command);

        if (command == EXIT) {
            return 0;
        }

        if (command == _FIRST) {
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