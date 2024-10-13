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
            
            int *array = (int *)malloc(n * sizeof(int));
            printf("Введите элементы массива:\n");
            for (int i = 0; i < n; i++) {
                printf("\n%d\n", i);
                int t;
                scanf("%d", &t);
                array[i] = t;
                printf("\n-%d-\n", array[i]);
            }
        } else {
            srand(time(NULL));
            int n = rand() % 1000;

            array = (int *)malloc(n * sizeof(int));
            for (int i = 0; i < n; i++) {
                array[i] = rand() % 10000;
            }
            printf("Исходный массив:\n");
            for (int i = 0; i < n; i++) {
                printf("%d ", array[i]);
            }
            printf("\n");
        }
        printf("syuck");
        MergeData data = {array, 0, n - 1};
        printf("fuck");
        ParallelSort((void*)(&data));
        
        printf("Отсортированный массив:\n");
        for (int i = 0; i < n; i++) {
            printf("%d ", array[i]);
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