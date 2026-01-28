#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "../../agregator.h"
#include "../../../messages/messages.h"

void* parent_handler_thread(void* arg) {
    Agregator* agregator = (Agregator*)arg;
    struct pollfd pfd;
    pfd.fd = agregator->parent_fd;
    pfd.events = POLLIN;

    while (agregator->shutdown == 0) {
        int ret = poll(&pfd, 1, 500);

        if (ret == 0) continue; 
        if (ret < 0) {
            if (agregator->shutdown) break;
            perror("Poll parent failed");
            break;
        }

        if (pfd.revents & POLLIN) {
            Response resp;
            int val_read = read(agregator->parent_fd, &resp, sizeof(Response));
            
            if (val_read <= 0) {
                printf("\n[Critical] connection to parent lost: %d\n", agregator->listener_port);
                agregator->shutdown = 1;
                pthread_cond_broadcast(&agregator->power_notify);
                break;
            }

            pthread_mutex_lock(&agregator->power_lock);
            agregator->waiting_for_parent = 0;
            if (resp.response_type == APPROVED) {
                agregator->available_power += resp.power_amount;
                printf("\n[Parent-Link] Received %.2f kW.\n", resp.power_amount);
            }
            pthread_cond_broadcast(&agregator->power_notify);
            pthread_mutex_unlock(&agregator->power_lock);
        }
    }
    printf("[Parent-Handler] Thread exited.\n");
    return NULL;
}