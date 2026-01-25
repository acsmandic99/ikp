#include "messages/messages.h"
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    int port;

    // Dozvoljavamo unos porta preko konzole ili koristimo tvoj default 52000
    printf("Unesite port agregatora (default 52000): ");
    if (scanf("%d", &port) != 1) port = 52000;

    if ((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        printf("\n Greška pri kreiranju socket-a \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\n Nevalidna adresa \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("\n Konekcija nije uspela");
        return -1;
    }

    printf("Povezan na agregator na portu %d.\n", port);
    printf("Komande: Unesite kolicinu kW koju zelite (0 za izlaz)\n");

    while (1) {
        float power_wanted;
        printf("\nKoliko kW zelite da zatrazite? ");
        if (scanf("%f", &power_wanted) != 1) break;

        if (power_wanted <= 0) break;

        Request req;
        req.request_type = REQUEST_POWER;
        req.power_amount = power_wanted;

        // Slanje zahteva
        if (send(sock, &req, sizeof(Request), 0) < 0) {
            perror("Slanje neuspelo");
            break;
        }
        printf("Zahtev poslat: %.2f kW. Cekam odgovor...\n", power_wanted);

        Response resp;
        int val_read = recv(sock, &resp, sizeof(Response), 0);
        
        if (val_read > 0) {
            if (resp.response_type == APPROVED) {
                printf(">>> ODOBRENO: Dobili ste %.2f kW struje!\n", resp.power_amount);
            } else if (resp.response_type == REJECTED) {
                printf(">>> ODBIJENO: Agregator trenutno nema dovoljno struje.\n");
            }
        } else if (val_read == 0) {
            printf("Server je zatvorio konekciju.\n");
            break;
        } else {
            perror("Greska pri prijemu odgovora");
            break;
        }
    }

    printf("Gasenje klijenta...\n");
    close(sock);
    return 0;
}