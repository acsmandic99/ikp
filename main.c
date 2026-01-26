#include "agregator/agregator.h"
#include <sched.h>
#include <stdio.h>
#include <unistd.h>
int main() {
    setvbuf(stdout, NULL, _IONBF, 0);
   printf("--- AGREGATOR TEST START ---\n");
    printf("\nUnesi broj porta nadredjenog");
    int port;
    scanf("%d",&port);
    printf("\nUnesi broj porta centralnog agregatora");
    int listener_port;
    scanf("%d",&listener_port);
    Agregator* aggr = init_agregator(0, 4, 100,listener_port,port);

    Agregator* pod_agregatori[10];
    for(int i = 0; i < 10; i++)
    {
        pod_agregatori[i] = init_agregator(i + 1, 4, 100, listener_port,listener_port + i );
        sleep(1);
    }

    if (!aggr) {
        printf("Inicijalizacija nije uspela!\n");
        return 1;
    }

    printf("Server podignut. Cekam klijente na portu 52000...\n");
    while(aggr->shutdown == 0) {
    sleep(1);
    }   
    destroy_agregator(aggr);
    return 0;
}