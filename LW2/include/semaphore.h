#ifndef _SEMAPHORE_H
#define _SEMAPHORE_H

#include <pthread.h>

typedef struct {
    int value;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} sem_t;

void sem_init(sem_t *sem, int value);

void sem_wait(sem_t *sem);

int sem_trywait(sem_t *sem);

void sem_post(sem_t *sem);

void sem_destroy(sem_t *sem);

#endif