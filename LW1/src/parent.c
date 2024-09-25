#include "parent.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

void Parent(const char* pathToChild, FILE* stream) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;

    char filename1[MAX_BUFFER];
    char filename2[MAX_BUFFER];
    char input[MAX_BUFFER];

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        exit(-1);
    }

    printf("Введите имя файла для первого дочернего процесса: ");
    fgets(filename1, MAX_BUFFER, stdin);
    filename1[strcspn(filename1, "\n")] = 0;

    printf("Введите имя файла для второго дочернего процесса: ");
    fgets(filename2, MAX_BUFFER, stdin);
    filename2[strcspn(filename2, "\n")] = 0;

    pid1 = fork();
    if (pid1 == 0) {
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        execl("./child1", "./child1", filename1, NULL);
        perror("execl failed");
        exit(-1);
    }

    pid2 = fork();
    if (pid2 == 0) {
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        execl("./child2", "./child2", filename2, NULL);
        perror("execl failed");
        exit(-1);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    while (1) {
        printf("Введите строку: ");
        fgets(input, MAX_BUFFER, stdin);
        input[strcspn(input, "\n")] = 0;

        if (strlen(input) % 2 == 1) {
            write(pipe1[1], input, strlen(input) + 1);
        } else {
            write(pipe2[1], input, strlen(input) + 1);
        }
        sleep(1);
    }

    close(pipe1[1]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);
}