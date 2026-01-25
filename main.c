#include "agregator/agregator.h"
#include <sched.h>
#include <stdio.h>
int main() {
   printf("--- AGREGATOR TEST START ---\n");
    printf("\nUnesi broj porta ");
    int port;
    scanf("%d",&port);
    Agregator* aggr = init_agregator(0, 4, 100,port);

    if (!aggr) {
        printf("Inicijalizacija nije uspela!\n");
        return 1;
    }

    printf("Server podignut. Cekam klijente na portu 52000...\n");
    scanf("%d");
    return 0;
}