#ifndef AGREGATOR
#define AGREGATOR
#include "../strukture/thread_pool/thread_pool.h"
#include "../strukture/hashmapa/hashmap.h"
#include <pthread.h>
#include "thread-ovi/parent_thread/parent_thread.h"


//hashmapa je thread-safe
//thread pool je thread-safe
//power lock nam sluzi da bezbedno pristupamo 
//available power

typedef struct Agregator{
    int listen_fd;
    int epoll_fd;
    
    ThreadPool* tp;     
    Hashmap* clients;   
    
    float available_power;      
    pthread_mutex_t power_lock;
    pthread_cond_t power_notify;
    int waiting_for_parent;
    
    int parent_fd; 
    

    int listener_port;
    int offset;//da nam port ne bude isti svugde

    int shutdown;

    pthread_t listener_tid;
    pthread_t epoll_tid;
    pthread_t parent_tid;
} Agregator;

Agregator* init_agregator(int offset,int num_threads,int queue_size,int listener_port,int parent_port);
void* start_epoll_loop(void* arg);
void destroy_agregator(Agregator* agregator);
#endif