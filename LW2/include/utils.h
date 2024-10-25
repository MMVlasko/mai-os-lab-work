#ifndef _UTILS_H
#define _UTILS_H

extern int maxCountOfThreads;

extern pthread_mutex_t mutex;
extern int countOfActiveThreads;

typedef struct {
    int *array;
    int left;
    int right;
} MergeData;

void Merge(int *array, int left, int mid, int right);

void *MergeSort(void *arg);

#endif