#include "../../../strukture/thread_pool/thread_pool.h"
#include "../../agregator.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

void* worker_thread(void* arg) {
    ThreadPool* tp = (ThreadPool*)arg; 
    struct Agregator* agregator = tp->agregator;
    char buffer[1024];

    while (1) {
        pthread_mutex_lock(&tp->lock);

        while (tp->queue_size == 0 && !tp->shutdown) {
            pthread_cond_wait(&tp->notify, &tp->lock);
        }

        if (tp->shutdown) {
            pthread_mutex_unlock(&tp->lock);
            break;
        }

        Request current_request = tp->queue[tp->head];
        tp->head = (tp->head + 1) % tp->queue_capacity;
        tp->queue_size--;
        printf("[Thread %lu] Preuzimam FD %d iz reda...\n", (unsigned long)pthread_self(), current_request.client_fd);
        pthread_mutex_unlock(&tp->lock);
        int spavam = rand()%10;
        sleep(spavam);
        
        int valread = recv(current_request.client_fd, buffer, sizeof(buffer) - 1, 0);
        
        if (valread > 0) {
            buffer[valread] = '\0';
            printf("[Thread %lu] FD %d poslao: %s\n", (unsigned long)pthread_self(), current_request.client_fd, buffer);            
            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            ev.data.fd = current_request.client_fd;
            
            if(epoll_ctl(agregator->epoll_fd, EPOLL_CTL_MOD, current_request.client_fd, &ev) == -1) {
                perror("epoll_ctl MOD failed");
            }
        } else if (valread == 0) {
            printf("[Thread %lu]  Klijent na FD %d se diskonektovao.\n", (unsigned long)pthread_self(),current_request.client_fd);
            
            Client* cs = hashmap_remove(agregator->clients, current_request.client_fd);
            pthread_mutex_lock(&agregator->power_lock);
            agregator->available_power += cs->current_power_usage;
            pthread_mutex_unlock(&agregator->power_lock);
            free(cs);
            close(current_request.client_fd);
        }
    }
    return NULL;
}
