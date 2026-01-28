#include "thread_pool.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../../agregator/agregator.h"
#include "../../agregator/thread-ovi/worker_thread/worker_thread.h"


ThreadPool* init_thread_pool(struct Agregator* agregator,int THREADS_NUM, int queue_capacity) {
    ThreadPool* tp = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (tp == NULL) return NULL;

    tp->threads_num = THREADS_NUM;
    tp->agregator = agregator;
    tp->queue_capacity = queue_capacity;
    tp->queue_size = 0;
    tp->head = 0;
    tp->tail = 0;
    tp->shutdown = 0;

    tp->queue = (ClientRequest*)malloc(queue_capacity * sizeof(ClientRequest));
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
                free((tp->threads));
                free(tp->queue);
                free(tp);
                return NULL;
            }
        }
    
    return tp;
}


int thread_pool_add_task(ThreadPool* tp, ClientRequest req) {

    pthread_mutex_lock(&tp->lock);
    if (tp->queue_size == tp->queue_capacity) {
        pthread_mutex_unlock(&tp->lock);
        return -1;
    }

    tp->queue[tp->tail] = req;
    tp->tail = (tp->tail + 1) % tp->queue_capacity;
    tp->queue_size++;

    pthread_cond_signal(&tp->notify);
    pthread_mutex_unlock(&tp->lock);
    return 0;
}

void thread_pool_destroy(ThreadPool* tp) {
    pthread_mutex_lock(&tp->lock);
    tp->shutdown = 1;
    pthread_cond_broadcast(&tp->notify);
    pthread_mutex_unlock(&tp->lock);

    for (int i = 0; i < tp->threads_num; i++) {
        pthread_join(tp->threads[i], NULL); 
    }
    printf("\n Unisteni worker threadovi\n");
    free(tp->threads);
    free(tp->queue);
    pthread_mutex_destroy(&tp->lock);
    pthread_cond_destroy(&tp->notify);
    free(tp);
}