#include <stdlib.h>

#include <semaphore.h>

void sem_init(sem_t *sem, int value) {
    sem->value = value;
    pthread_mutex_init(&sem->mutex, NULL);
    pthread_cond_init(&sem->cond, NULL);
}

void sem_wait(sem_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    while (sem->value <= 0) {
        pthread_cond_wait(&sem->cond, &sem->mutex);
    }
    --sem->value;
    pthread_mutex_unlock(&sem->mutex);
}

int sem_trywait(sem_t *sem) {
    int result = 0;

    pthread_mutex_lock(&sem->mutex);
    if (sem->value > 0) {
        --sem->value;
        result = 1;
    } else {
        result = 0;
    }
    pthread_mutex_unlock(&sem->mutex);

    return result;
}

void sem_post(sem_t *sem) {
    pthread_mutex_lock(&sem->mutex);
    ++sem->value;
    pthread_cond_signal(&sem->cond);
    pthread_mutex_unlock(&sem->mutex);
}

void sem_destroy(sem_t *sem) {
    pthread_mutex_destroy(&sem->mutex);
    pthread_cond_destroy(&sem->cond);
}