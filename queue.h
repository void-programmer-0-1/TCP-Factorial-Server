#ifndef _QUEUE_H_
#define _QUEUE_H_

#include <pthread.h>

#define QUEUE_MAX_CAPACITY 10000

typedef struct{
    int buffer[QUEUE_MAX_CAPACITY];
    int head;
    int tail;
    int count;
    pthread_mutex_t m;
    pthread_cond_t nonempty;
    pthread_cond_t nonfull;
    int shutdown;
}socket_queue_t;

void queue_init(socket_queue_t *queue);
int queue_push(socket_queue_t *queue, int clientfd); // 1=ok,0=full,-1=shutting down
int queue_pop(socket_queue_t *queue, int *clientfd); // 1=ok,0=shutdown

#endif