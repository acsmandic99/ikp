#include "thread_pool.h"
#include <stdio.h>
#include <stdlib.h>


ThreadPool* init_thread_pool(int THREADS_NUM, int queue_capacity) {
    ThreadPool* tp = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (tp == NULL) return NULL;

    tp->threads_num = THREADS_NUM;
    tp->queue_capacity = queue_capacity;
    tp->queue_size = 0;
    tp->head = 0;
    tp->tail = 0;
    tp->shutdown = 0;

    tp->queue = (int*)malloc(queue_capacity * sizeof(int));
    if(tp->queue == NULL)
    {
        free(tp);
        return NULL;
    }
    
    tp->threads = (pthread_t*)malloc(THREADS_NUM * sizeof(pthread_t));
    if(tp->threads == NULL)
    {
        free(tp->queue);
        free(tp);
        return NULL;
    }

    pthread_mutex_init(&(tp->lock), NULL);
    pthread_cond_init(&(tp->notify), NULL);

    for (int i = 0; i < THREADS_NUM; i++) {
        if (pthread_create(&(tp->threads[i]), NULL, worker_thread, (void*)tp) != 0) {
            free(tp->threads);
            free(tp->queue);
            free(tp);
            return NULL;
        }
    }
    return tp;
}

void* worker_thread(void* args)
{
    
}