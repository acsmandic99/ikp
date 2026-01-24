#ifndef EPOLL
#define EPOLL
#include "strukture/hashmapa/hashmap.h"


typedef struct {
    Hashmap* hm;
    int epoll_fd;
    int id;
} ServerContext;

void* epoll_thread_func(void* arg);
#endif