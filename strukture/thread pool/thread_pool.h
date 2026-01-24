#include <pthread.h>


typedef struct ThreadPool
{
    pthread_t* threads;   
    int threads_num;     
    int* queue;               
    int queue_capacity;
    int queue_size;
    int head;                 
    int tail;
    
    pthread_mutex_t lock;      
    pthread_cond_t notify;     
    int shutdown;              
}ThreadPool;


ThreadPool* init_thread_pool(int THREADS_NUM,int queue_capacity);
void* worker_thread(void* args);