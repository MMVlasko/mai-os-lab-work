#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#include <utils.h>

void Merge(int *array, int leftEdge, int mid, int rightEdge) {
    int leftArrayVolume = mid - leftEdge + 1;
    int rightArrayVolume = rightEdge - mid;
    int *leftArray = (int *)malloc(leftArrayVolume * sizeof(int));
    int *rightArray = (int *)malloc(rightArrayVolume * sizeof(int));
    
    for (int i = 0; i < leftArrayVolume; i++) leftArray[i] = array[leftEdge + i];
    for (int i = 0; i < rightArrayVolume; i++) rightArray[i] = array[mid + 1 + i];
    
    int i = 0, j = 0, k = leftEdge;
    while (i < leftArrayVolume && j < rightArrayVolume) {
        if (leftArray[i] <= rightArray[j]) {
            array[k++] = leftArray[i++];
        } else {
            array[k++] = rightArray[j++];
        }
    }
    while (i < leftArrayVolume) array[k++] = leftArray[i++];
    while (j < rightArrayVolume) array[k++] = rightArray[j++];
    
    free(leftArray);
    free(rightArray);
}

void Sort(MergeData *data) {
    if (data->left < data->right) {
        int mid = data->left + (data->right - data->left) / 2;
        MergeData firstData = {data->array, data->left, mid};
        MergeData secondData = {data->array, mid + 1, data->right};
        Sort(&firstData);
        Sort(&secondData);
        Merge(data->array, data->left, mid, data->right);
    }
}

void *ParallelSort(void *arg) {
    MergeData *data = (MergeData*)arg;
    int leftEdge = data->left;
    int rightEdge = data->right;
    int *array = data->array;

    if (leftEdge < rightEdge) {
        int mid = leftEdge + (rightEdge - leftEdge) / 2;
        pthread_t firstThread, secondThread;
        MergeData firstData = {array, leftEdge, mid};
        MergeData secondData = {array, mid + 1, rightEdge};

        if (!sem_trywait(&semaphore)) {
            pthread_create(&firstThread, NULL, ParallelSort, &firstData);
        } else {
            Sort(&firstData);
        }
        printf("--%d--", mid);
        fflush(stdout);
        if (!sem_trywait(&semaphore)) {
            pthread_create(&secondThread, NULL, ParallelSort, &secondData);
        } else {
            Sort(&secondData);
        }

        if (!sem_trywait(&semaphore)) {
            pthread_join(firstThread, NULL);
        }

        if (!sem_trywait(&semaphore)) {
            pthread_join(secondThread, NULL);
        }

        Merge(array, leftEdge, mid, rightEdge);

        sem_post(&semaphore);
        sem_post(&semaphore);
    }
    return NULL;
}