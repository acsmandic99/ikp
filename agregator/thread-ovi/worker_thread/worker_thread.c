#include "../../../strukture/thread_pool/thread_pool.h"
#include "../../agregator.h"
#include <stdio.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../../../messages/messages.h"


void send_request_for_more_power(Agregator* agregator,float power_amount);
void send_approved_response_to_client(int client_fd,float power_approved);
void send_rejected_response_to_client(int client_fd);

void* worker_thread(void* arg) {
    ThreadPool* tp = (ThreadPool*)arg; 
    struct Agregator* agregator = tp->agregator;

    while (1) {
        pthread_mutex_lock(&tp->lock);

        while (tp->queue_size == 0 && !tp->shutdown) {
            pthread_cond_wait(&tp->notify, &tp->lock);
        }

        if (tp->shutdown) {
            pthread_mutex_unlock(&tp->lock);
            break;
        }

        ClientRequest current_request = tp->queue[tp->head];
        tp->head = (tp->head + 1) % tp->queue_capacity;
        tp->queue_size--;
        fflush(stdout);
        printf("\n[Thread %lu] Preuzimam FD %d iz reda...", (unsigned long)pthread_self(), current_request.client_fd);
        fflush(stdout);

        pthread_mutex_unlock(&tp->lock);
        int spavam = rand()%10;
        sleep(spavam);
        Request req;
        int valread = recv(current_request.client_fd, &req, sizeof(Request), 0);
        
        if (valread > 0) {
            if(req.request_type == REQUEST_POWER)
            {
                printf("\n[Thread %lu] FD %d traži %fkW", (unsigned long)pthread_self(), current_request.client_fd, req.power_amount);
                fflush(stdout);

                
                int approved = 0; 

                pthread_mutex_lock(&agregator->power_lock);
                if(agregator->available_power < req.power_amount && agregator->waiting_for_parent == 0)
                {
                    printf("\n[Worker] Nema dovoljno struje, pitam roditelja...\n");
                    fflush(stdout);
                    send_request_for_more_power(agregator, req.power_amount * 5);
                    agregator->waiting_for_parent = 1;
                }

                while(agregator->available_power < req.power_amount && agregator->waiting_for_parent == 1)
                {
                    pthread_cond_wait(&agregator->power_notify, &agregator->power_lock);
                }

                if(agregator->available_power >= req.power_amount)
                {
                    agregator->available_power -= req.power_amount;
                    approved = 1;
                }
                
                pthread_mutex_unlock(&agregator->power_lock);

                if(approved)
                {
                    Client* c = (Client*)get_value(agregator->clients, current_request.client_fd);
                    if(c) c->current_power_usage += req.power_amount;
                    printf("\n[Thread %lu] klijentu FD %d odobreno %.2fkW struje",(unsigned long)pthread_self(), current_request.client_fd,req.power_amount);
                    fflush(stdout);
                    send_approved_response_to_client(current_request.client_fd, req.power_amount);
                }
                else 
                {
                    fflush(stdout);
                    printf("\n[Worker] Zahtev za FD %d odbijen (nema resursa).", current_request.client_fd);
                    fflush(stdout);
                    send_rejected_response_to_client(current_request.client_fd);
                }
            }

            struct epoll_event ev;
            ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
            ev.data.fd = current_request.client_fd;
            if(epoll_ctl(agregator->epoll_fd, EPOLL_CTL_MOD, current_request.client_fd, &ev) == -1) {
                perror("epoll_ctl MOD failed");
            }
        } else if (valread == 0) {
            printf("\n[Thread %lu] Klijent na FD %d se diskonektovao.\n", (unsigned long)pthread_self(), current_request.client_fd);
            fflush(stdout);
            Client* cs = (Client*)get_value(agregator->clients, current_request.client_fd);
            
            // Proveravamo da li klijent postoji I da li je to onaj isti koji je poslao ovaj zahtev
            if(cs != NULL && cs->client_id == current_request.client_id) {
                hashmap_remove(agregator->clients, current_request.client_fd);
                
                pthread_mutex_lock(&agregator->power_lock);
                agregator->available_power += cs->current_power_usage;
                pthread_cond_broadcast(&agregator->power_notify);
                pthread_mutex_unlock(&agregator->power_lock);
                
                free(cs);
            }
            close(current_request.client_fd);
        }
    }
    return NULL;
}


void send_request_for_more_power(Agregator* agregator,float power_amount)
{
    Request req;
    req.request_type = REQUEST_POWER;
    req.power_amount = power_amount;
    write(agregator->parent_fd, &req, sizeof(Request));
}

void send_approved_response_to_client(int client_fd,float power_approved)
{
    Response resp;
    resp.response_type = APPROVED;
    resp.power_amount = power_approved;
    write(client_fd,&resp,sizeof(Response));
}

void send_rejected_response_to_client(int client_fd)
{
    Response resp;
    resp.response_type = REJECTED;
    resp.power_amount = 0;
    write(client_fd,&resp,sizeof(Response));
}

