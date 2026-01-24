#include "strukture/hashmapa/hashmap.h"
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "listener_thread.h"
#include <fcntl.h>
#define PORT 52000

void* create_listener_thread_func(void* arg)
{
    ServerContext* args = (ServerContext*)arg;
    Hashmap* hm = args->hm;
    int i = args->id;
    int epoll_fd = args->epoll_fd;
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
        exit(EXIT_FAILURE);
    }
    
    while(1)
    {

        if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
        {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        int* client_socket = (int*)malloc(sizeof(new_socket));
        if(client_socket == NULL)
        {
            printf("\nAn error occured while taking memory for new socket\n");
            break;
        }
        char* klijent_ip = inet_ntoa(address.sin_addr);
        int klijent_port = ntohs(address.sin_port);
        printf("\nNew client accepted.Client ip address:%s:%d\n",klijent_ip,klijent_port);
        *client_socket = new_socket;

        hash_map_put(hm, new_socket, client_socket);

        fcntl(new_socket, F_SETFL, O_NONBLOCK);
        
        struct epoll_event ev;
        ev.events = EPOLLIN | EPOLLET;   
        ev.data.fd = new_socket;

        if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_socket, &ev) == -1)
        {
            perror("epoll_ctl ADD failed");
            close(new_socket);
            free(client_socket);
        }
    }
    printf("\nClosing listener...");
    close(new_socket);
    close(server_fd);
}