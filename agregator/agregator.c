#include <pthread.h>
#include "../strukture/hashmapa/hashmap.h"
#include "../epoll_thread.h"
#include "../listener_thread.h"
#include <stdio.h>
#include <sys/epoll.h>

void init_agregator()
{
    pthread_t listener_tid, epoll_tid;

    Hashmap* hm = init_hashmap(0);

    ServerContext* args = malloc(sizeof(ServerContext));
    args->hm = hm;
    args->id = 1;

    args->epoll_fd = epoll_create1(0);
    if(args->epoll_fd == -1){
        perror("epoll_create1");
        return;
    }

    pthread_create(&epoll_tid, NULL, epoll_thread_func, args);
    pthread_create(&listener_tid, NULL, create_listener_thread_func, args);

    pthread_join(listener_tid,NULL);
    pthread_join(epoll_tid,NULL);

    return;
}