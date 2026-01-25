#include "../../../strukture/hashmapa/hashmap.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "listener_thread.h"
#include <fcntl.h>
#include "../../agregator.h"

#define PORT 52000

void* create_listener_thread_func(void* arg)
{
    struct Agregator* agregator = (struct Agregator*)arg;
    Hashmap* hm = agregator->clients;
    int i = agregator->i;
    int epoll_fd = agregator->epoll_fd;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    int port = PORT + i;
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    printf("\nListener thread created.\nHe listens on:%s:%d\n",inet_ntoa(address.sin_addr),port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    
    if(listen(server_fd, 3) < 0)
    {
        perror("Listen");
        return NULL;
    }
    
    while(1)
    {

        struct sockaddr_in address;
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        
        if (new_socket < 0) {
            perror("Accept failed");
            continue; 
        }
        fcntl(new_socket, F_SETFL, O_NONBLOCK);

        Client* cs = (Client*)malloc(sizeof(Client));
        cs->client_fd = new_socket;
        cs->client_id = rand(); 
        cs->current_power_usage = 0; 

        hash_map_put(agregator->clients, new_socket, cs);

        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;   
        ev.data.fd = new_socket; 

        if(epoll_ctl(agregator->epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
            perror("epoll_ctl ADD failed");
            hashmap_remove(agregator->clients, new_socket);
            close(new_socket);
            free(cs);
        } else {
            printf("\nNovi klijent na FD %d spreman za radnike.\n", new_socket);
        }
    }
    return NULL;
}