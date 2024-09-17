// parent.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_BUFFER 256

void parent(const char* pathToChild, FILE* stream) {
    int pipe1[2], pipe2[2];
    pid_t pid1, pid2;
    char filename1[MAX_BUFFER], filename2[MAX_BUFFER];
    char input[MAX_BUFFER];

    // Создание каналов
    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("Pipe failed");
        exit(-1);
    }

    // Ввод имени файлов для двух дочерних процессов
    printf("Введите имя файла для первого дочернего процесса: ");
    fgets(filename1, MAX_BUFFER, stdin);
    filename1[strcspn(filename1, "\n")] = 0;  // Удаление символа новой строки

    printf("Введите имя файла для второго дочернего процесса: ");
    fgets(filename2, MAX_BUFFER, stdin);
    filename2[strcspn(filename2, "\n")] = 0;

    // Создание первого дочернего процесса
    pid1 = fork();
    if (pid1 == 0) {
        // Закрываем канал для записи, оставляем только чтение
        close(pipe1[1]);
        dup2(pipe1[0], STDIN_FILENO);
        close(pipe1[0]);

        // Запуск программы первого дочернего процесса
        execl("./child1", "./child1", filename1, NULL);
        perror("execl failed");
        exit(-1);
    }

    // Создание второго дочернего процесса
    pid2 = fork();
    if (pid2 == 0) {
        // Закрываем канал для записи, оставляем только чтение
        close(pipe2[1]);
        dup2(pipe2[0], STDIN_FILENO);
        close(pipe2[0]);

        // Запуск программы второго дочернего процесса
        execl("./child2", "./child2", filename2, NULL);
        perror("execl failed");
        exit(-1);
    }

    // Родительский процесс
    close(pipe1[0]);
    close(pipe2[0]);

    // Ввод строк и передача в дочерние процессы
    while (1) {
        printf("Введите строку: ");
        fgets(input, MAX_BUFFER, stdin);
        input[strcspn(input, "\n")] = 0;  // Удаление символа новой строки

        if (strlen(input) % 2 == 1) {
            // Нечётная длина — отправляем в первый дочерний процесс
            write(pipe1[1], input, strlen(input) + 1);
        } else {
            // Чётная длина — отправляем во второй дочерний процесс
            write(pipe2[1], input, strlen(input) + 1);
        }
        sleep(1);
    }

    close(pipe1[1]);
    close(pipe2[1]);

    // Ожидание завершения дочерних процессов
    wait(NULL);
    wait(NULL);
}