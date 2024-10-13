#ifndef _UTILS_H
#define _UTILS_H

#include <semaphore.h>

static sem_t semaphore;

typedef struct {
    int *array;
    int left;
    int right;
} MergeData;

void Merge(int *array, int left, int mid, int right);

void Sort(MergeData *data);

void *ParallelSort(void *arg);

#endif