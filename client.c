#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 52000
#define SERVER_IP "127.0.0.1"

int main(int argc, char const *argv[]) {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char *hello = "Pozdrav od klijenta!";
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Greška pri kreiranju socket-a \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        printf("\n Nevalidna adresa / Adresa nije podržana \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\n Konekcija nije uspela \n");
        return -1;
    }

    send(sock, hello, strlen(hello), 0);
    printf("Poruka poslata serveru: %s\n", hello);

    sleep(2);

    close(sock);
    return 0;
}