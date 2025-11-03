#include "queue.h"
#include <pthread.h>

void queue_init(socket_queue_t* queue){
    queue->head = queue->tail = queue->count = 0;
    queue->shutdown = 0;
    pthread_mutex_init(&queue->m, NULL);
    pthread_cond_init(&queue->nonempty, NULL);
    pthread_cond_init(&queue->nonfull, NULL);
}

// this api is used only by the master 
int queue_push(socket_queue_t* queue, int clientfd){
    pthread_mutex_lock(&queue->m);

    if(queue->shutdown){
        pthread_mutex_unlock(&queue->m);
        return -1;
    }

    if(queue->count >= QUEUE_MAX_CAPACITY){
        pthread_mutex_unlock(&queue->m);
        return 0;
    }

    queue->buffer[queue->tail++] = clientfd;
    pthread_cond_signal(&queue->nonempty);
    pthread_mutex_unlock(&queue->m);
    return 0;
}


// this api is used only by the workers
int queue_pop(socket_queue_t* queue, int *clientfd) {
    pthread_mutex_lock(&queue->m);

    // current thread waits/suspends/sleeps untill 
    // a nonempty signal is trigger from the master
    while(queue->count == 0 && !queue->shutdown){
        pthread_cond_wait(&queue->nonempty, &queue->m);
    }

    if(queue->count == 0 && queue->shutdown){
        pthread_mutex_unlock(&queue->m);
    }

    *clientfd = queue->buffer[queue->head];
    queue->head = (queue->head + 1) % QUEUE_MAX_CAPACITY;
    queue->count--;
    pthread_cond_signal(&queue->nonfull);
    pthread_mutex_unlock(&queue->m);
    return 0;
}

