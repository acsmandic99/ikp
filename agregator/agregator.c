#include "thread-ovi/parent_thread/parent_thread.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../strukture/hashmapa/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "agregator.h"
#include "../messages/messages.h"


#define MAX_EVENTS 9999

void agregator_connect_to_parent(Agregator* agregator,int port);

Agregator* init_agregator(int offset,int num_threads,int queue_size,int parent_port) {
    Agregator* agregator = (Agregator*)malloc(sizeof(Agregator));
    if (!agregator) return NULL;
    
    agregator->i = offset;
    agregator->waiting_for_parent = 0;
    agregator_connect_to_parent(agregator,parent_port);
    if(agregator->parent_fd == -1)
    {
        printf("\nSomething went wrong...");
        printf("\nShutting down...");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&agregator->power_lock, NULL);
    pthread_cond_init(&agregator->power_notify,NULL);

    agregator->clients = init_hashmap(101); 

    agregator->epoll_fd = epoll_create1(0);
    if (agregator->epoll_fd == -1) {
        perror("epoll_create1");
        return NULL;
    }
    agregator->tp = init_thread_pool(agregator, num_threads, queue_size);

    pthread_t parent_handler_tid;
    pthread_create(&parent_handler_tid,NULL,parent_handler_thread,(void*)agregator);
    pthread_detach(parent_handler_tid);

    pthread_t listener_tid;
    pthread_create(&listener_tid, NULL, create_listener_thread_func, (void*)agregator);
    pthread_detach(listener_tid);

    pthread_t epoll_tid;
    pthread_create(&epoll_tid, NULL, start_epoll_loop, (void*)agregator);
    pthread_detach(epoll_tid);
    
    return agregator;
}

void agregator_connect_to_parent(Agregator* agregator,int port)
{
    int sock = 0;
    struct sockaddr_in parent_addr;
    char buffer[1024] = {0};
    agregator->parent_fd = -1;
    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("\n Error while making socket \n");
        return;
    }

    parent_addr.sin_family = AF_INET;
    parent_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, "127.0.0.1", &parent_addr.sin_addr) <= 0) {
        printf("\n Address error \n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&parent_addr, sizeof(parent_addr)) < 0) {
        printf("\n Connection to parent failed \n");
        return;
    }
    agregator->parent_fd = sock;
    Request req;
    req.request_type = REQUEST_POWER;
    req.power_amount = 200;
    write(agregator->parent_fd,&req,sizeof(Request));
    Response resp;
    int val_read = read(agregator->parent_fd, &resp,sizeof(Response));
    if(resp.response_type == APPROVED)
    {
        agregator->available_power = resp.power_amount;
        printf("\nApproved power: %f\n",resp.power_amount);
    }
    else {
        printf("\nRequest rejected...");
        printf("\nShutting down...");
        exit(EXIT_FAILURE);
    }
}

void* start_epoll_loop(void* arg) {
    Agregator* agregator = (Agregator*)arg;
    struct epoll_event events[MAX_EVENTS];
    printf("[System] Epoll loop started...\n");

    while (1) {
        int nfds = epoll_wait(agregator->epoll_fd, events, MAX_EVENTS, -1);
        
        for (int i = 0; i < nfds; i++) {
            ClientRequest req;
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