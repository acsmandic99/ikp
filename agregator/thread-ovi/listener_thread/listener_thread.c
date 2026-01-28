#include "../../../strukture/hashmapa/hashmap.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "listener_thread.h"
#include <fcntl.h>
#include "../../agregator.h"
#include <poll.h>

void* create_listener_thread_func(void* arg)
{
    struct Agregator* agregator = (struct Agregator*)arg;
    Hashmap* hm = agregator->clients;
    int offset = agregator->offset;
    int epoll_fd = agregator->epoll_fd;
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;
    int port = agregator->listener_port + offset;
    if((server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }
    agregator->listen_fd = server_fd;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    printf("\nListener thread created.\nHe listens on:%s:%d\n",inet_ntoa(address.sin_addr),port);
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    
    if(listen(server_fd, 3) == -1)
    {
        perror("Listen");
        return NULL;
    }

    long long id = 0;

    struct pollfd pfd;
    pfd.fd = server_fd;
    pfd.events = POLLIN; 

    while(agregator->shutdown == 0) {
        int ret = poll(&pfd, 1, 500);

        if (ret < 0) {
            if (agregator->shutdown) break;
            perror("Poll failed");
            break;
        }

        if (ret == 0) {
            continue;
        }

        if (pfd.revents & POLLIN) {
            struct sockaddr_in client_address;
            int client_addrlen = sizeof(client_address);
            int new_socket = accept(server_fd, (struct sockaddr*)&client_address, (socklen_t*)&client_addrlen);
            
            if (new_socket < 0) {
                if (agregator->shutdown) break;
                perror("Accept failed");
                continue; 
            }

            fcntl(new_socket, F_SETFL, O_NONBLOCK);

            Client* cs = (Client*)malloc(sizeof(Client));
            cs->client_fd = new_socket;
            cs->client_id = ++id; 
            cs->current_power_usage = 0; 

            hash_map_put(agregator->clients, new_socket, cs);

            struct epoll_event ev;
            memset(&ev, 0, sizeof(ev));
            ev.events = EPOLLIN | EPOLLET | EPOLLONESHOT;   
            ev.data.fd = new_socket; 

            if(epoll_ctl(agregator->epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1) {
                perror("epoll_ctl ADD failed");
                hashmap_remove_and_data(agregator->clients, new_socket);
                close(new_socket);
                if(cs != NULL) free(cs);
            } else {
                printf("\nNovi klijent na FD %d spreman za radnike.\n", new_socket);
            }
        }
    }
    
    close(server_fd);
    printf("[Listener] Thread exited cleanly.\n");
    return NULL;
}