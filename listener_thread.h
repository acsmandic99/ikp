#ifndef LISTENER
#define LISTENER
#include "strukture/hashmapa/hashmap.h"
#include "epoll_thread.h"



void* create_listener_thread_func(void* arg);
#endif