#include <stdio.h>

#include <headers.h>

typedef enum {
    EXIT = -1,
    PI = 1,
    EXP = 2
} Command;

int main(void) {
    printf(
        "Usage:\n\t\b-1 - exit\n"
        "\t1 - find Pi\n\t2 - find E\n"
    );

    while (1) {
        printf("command> ");
        int command;
        scanf("%d", &command);

        if (command == EXIT) {
            return 0;
        }

        if (command == PI) {
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