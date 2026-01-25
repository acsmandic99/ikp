// main.c
// #include "agregator/agregator.h"
// #include <stdio.h>
#include "agregator/agregator.h"
#include "strukture/hashmapa/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
   printf("--- AGREGATOR TEST START ---\n");

    Agregator* aggr = init_agregator(0, 4, 100);

    if (!aggr) {
        printf("Inicijalizacija nije uspela!\n");
        return 1;
    }

    printf("Server podignut. Cekam klijente na portu 52000...\n");
    scanf("%d");
    return 0;
}