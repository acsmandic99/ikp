#include <pthread.h>
#include "../strukture/hashmapa/hashmap.h"
#include <stdio.h>
#include <sys/epoll.h>
#include "agregator.h"

#define MAX_EVENTS 9999

Agregator* init_agregator(int offset,int num_threads,int queue_size) {
    Agregator* agregator = (Agregator*)malloc(sizeof(Agregator));
    if (!agregator) return NULL;
    
    agregator->i = offset;
    agregator->available_power = 1000.0; 
    pthread_mutex_init(&agregator->power_lock, NULL);

    agregator->clients = init_hashmap(100); 

    agregator->epoll_fd = epoll_create1(0);
    if (agregator->epoll_fd == -1) {
        perror("epoll_create1");
        return NULL;
    }
    agregator->tp = init_thread_pool(agregator, num_threads, queue_size);

    pthread_t listener_tid;
    pthread_create(&listener_tid, NULL, create_listener_thread_func, (void*)agregator);
    pthread_detach(listener_tid);

    pthread_t epoll_tid;
    pthread_create(&epoll_tid, NULL, start_epoll_loop, (void*)agregator);
    pthread_detach(epoll_tid);
    
    return agregator;
}

void* start_epoll_loop(void* arg) {
    Agregator* agregator = (Agregator*)arg;
    struct epoll_event events[MAX_EVENTS];
    printf("[System] Epoll loop started...\n");

    while (1) {
        int nfds = epoll_wait(agregator->epoll_fd, events, MAX_EVENTS, -1);
        
        for (int i = 0; i < nfds; i++) {
            Request req;
            req.client_fd = events[i].data.fd;
            
            Client* c = (Client*)get_value(agregator->clients, req.client_fd);
            if (c) {
                req.client_id = c->client_id;
                
                if (thread_pool_add_task(agregator->tp, req) != 0) {
                    printf("[Warn] Queue full, dropping request from FD %d\n", req.client_fd);
                }
            }
        }
    }
}