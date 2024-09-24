#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "utils.h"

const int MAX_BUFFER = 256;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    char buffer[MAX_BUFFER];
    FILE *file = fopen(argv[1], "w");
    if (!file) {
        perror("Failed to open file");
        exit(1);
    }

    while (read(STDIN_FILENO, buffer, MAX_BUFFER) > 0) {
        if (strlen(buffer) > 0) {
            ReverseString(buffer);
            printf("Второй дочерний процесс: %s\n", buffer);
            fflush(stdout);
            fprintf(file, "%s\n", buffer);
        }
    }

    fclose(file);
    return 0;
}