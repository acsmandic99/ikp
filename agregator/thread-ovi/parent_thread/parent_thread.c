#include "../../agregator.h"
#include "../../../messages/messages.h"
#include <pthread.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include "parent_thread.h"

void* parent_handler_thread(void* arg)
{
    Agregator* agregator = (Agregator*)arg;
    while(1)
    {
        Response resp;
        int val_read = read(agregator->parent_fd,&resp, sizeof(Response));
        
        if(val_read <= 0)
        {
            printf("\n[Critical] connection to parent is lost!\n");
            printf("\nSHUTTING DOWN GRACEFULLY\n");
            agregator->waiting_for_parent = 0;
            agregator->shutdown = 1;
            pthread_cond_broadcast(&agregator->power_notify);
            break;
        }

        if(resp.response_type == APPROVED)
        {
            pthread_mutex_lock(&agregator->power_lock);
            agregator->waiting_for_parent = 0;
            agregator->available_power += resp.power_amount;
            printf("\n[Parent-Link] Received %.2f kW from parent.\n", resp.power_amount);
            pthread_cond_broadcast(&agregator->power_notify);
            pthread_mutex_unlock(&agregator->power_lock);
        }
        else {
            pthread_mutex_lock(&agregator->power_lock);
            agregator->waiting_for_parent = 0;
            printf("\n[Parent-Link] Request was rejected!.\n");
            pthread_cond_broadcast(&agregator->power_notify);
            pthread_mutex_unlock(&agregator->power_lock);
        }
    }
    return NULL;
}