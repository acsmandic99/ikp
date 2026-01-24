#include "strukture/hashmapa/hashmap.h"
#include <stdio.h>
#include <string.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include "epoll_thread.h"

#define MAX_EVENTS 99999
void* epoll_thread_func(void* arg)
{
    ServerContext* sc = (ServerContext*)arg;
    struct epoll_event events[MAX_EVENTS];
    
    // OVE VARIJABLE MORAJU BITI VAN WHILE PETLJE
    int total_bytes = 0;
    char poruka[2048] = {0}; 
    char buffer[1024];

    while(1)
    {
        // Program ovde spava dok ne stigne novi komad podataka
        int nfds = epoll_wait(sc->epoll_fd, events, MAX_EVENTS, -1);

        for(int i = 0; i < nfds; i++)
        {
            int client_fd = events[i].data.fd;

            if(events[i].events & EPOLLIN)
            {
                int valread = recv(client_fd, buffer, sizeof(buffer)-1, 0);

                if(valread <= 0) {
                    close(client_fd);
                    total_bytes = 0; // Reset ako se klijent otkaci
                    continue;
                }

                // Dodajemo primljeno na kraj onoga što već imamo
                memcpy(poruka + total_bytes, buffer, valread);
                total_bytes += valread;
                poruka[total_bytes] = '\0'; // Uvek terminiraj string

                // PROVERA: Da li je stigao znak za kraj reda?
                if(poruka[total_bytes - 1] == '\n')
                {
                    printf("Klijent poslao sve odjednom: %s", poruka);

                    char* odgovor = "Primljeno!\n";
                    send(client_fd, odgovor, strlen(odgovor), 0);

                    // RESETUJEMO samo kad ispišemo celu liniju
                    total_bytes = 0;
                    memset(poruka, 0, sizeof(poruka));
                }
                else 
                {
                    // Ovo se desi kad stigne npr. "cao" bez \n
                    printf("Stigao deo poruke, cekam nastavak...\n");
                }
            }
        }
    }
}