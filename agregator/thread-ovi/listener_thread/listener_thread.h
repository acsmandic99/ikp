#ifndef LISTENER
#define LISTENER


struct Agregator;

typedef struct Client{
    int client_fd;
    long long client_id;
    float current_power_usage;
}Client;

void* create_listener_thread_func(void* arg);
#endif