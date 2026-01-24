#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 52000

int main()
{   
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    int opt = 1;

    // 1. Socket creation
    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Opciono: Dodaj ovo da izbegnes "Address already in use" gresku
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // 2. BIND - OVO JE FALILO
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // 3. Listen
    if(listen(server_fd, 3) < 0)
    {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    printf("Server slusa na portu %d..\n", PORT);
    // 4. Accept - POPRAVLJEN USLOV (treba < 0 za gresku)
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0)
    {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Klijent povezan!\n");

    char buffer[1024] = {0};
    int valread = recv(new_socket, buffer, 1024, 0);

    if(valread > 0){
        printf("Klijent poslao: %s\n", buffer);

        char* odgovor = "Poruka primljena!";
        send(new_socket, odgovor, strlen(odgovor), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}