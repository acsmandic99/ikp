#ifndef THREADPOOL
#define THREADPOOL

#include <pthread.h>
#include "../../agregator/thread-ovi/listener_thread/listener_thread.h"

struct Agregator;


typedef struct Request
{
    int client_fd;
    long long client_id;
}Request;

typedef struct ThreadPool
{
    pthread_t* threads;   
    int threads_num;     
    Request* queue;        
    struct Agregator* agregator;       
    int queue_capacity;
    int queue_size;
    int head;                 
    int tail;
    
    pthread_mutex_t lock;      
    pthread_cond_t notify;     
    int shutdown;              
}ThreadPool;


ThreadPool* init_thread_pool(struct Agregator* agregator,int THREADS_NUM, int queue_capacity);
int thread_pool_add_task(ThreadPool* tp, Request req);
#endif