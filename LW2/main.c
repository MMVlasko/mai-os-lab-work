#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <utils.h>

int main(int argc, char* argv[]) {
    if (argc == 3) {
        int *array;
        int n;
        sem_init(&semaphore, 0, atoi(argv[1]));

        if (*argv[2] == 'i') {
            printf("Введите количество элементов в массиве: ");
            scanf("%d", &n);
            
            array = (int*)malloc(n * sizeof(int));
            printf("Введите элементы массива:\n");
            for (int i = 0; i < n; i++) {
                scanf("%d", &array[i]);
            }
        } else {
            srand(time(NULL));
            n = rand() % 1000;

            array = (int *)malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                array[i] = rand() % 10000;
            }
            printf("Исходный массив:\n");
            for (int i = 0; i < n; i++) {
                printf("%d\n", array[i]);
            }
            printf("\n");
        }
        
        MergeData data = {array, 0, n - 1};
        ParallelSort(&data);
        
        printf("Отсортированный массив:\n");
        for (int i = 0; i < n; i++) {
            printf("%d\n", array[i]);
        }
        printf("\n");
        
        free(array);
        sem_destroy(&semaphore);
    } else {
        printf("Использование: ./lab2 <максимальное количество потоков> <i/r>\ni - ввести ");
        printf("произвольный массив\nr - сгенерировать случайный массив");
    }

    return 0;
}