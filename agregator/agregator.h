#ifndef AGREGATOR
#define AGREGATOR
#include "../strukture/thread_pool/thread_pool.h"
#include "../strukture/hashmapa/hashmap.h"


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
    
    int parent_fd; 
    int i;//da nam port ne bude isti svugde
} Agregator;

Agregator* init_agregator(int offset,int num_threads,int queue_size);
void* start_epoll_loop(void* arg);
#endif