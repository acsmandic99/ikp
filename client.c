#include <netinet/in.h>
#include <stdlib.h>
#include "stdio.h"
#include "strukture/hashmapa/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 52001

int main()
{   
    int sock = 0;
    struct sockaddr_in serv_addr;

    if((sock = socket(AF_INET,SOCK_STREAM,0))< 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if(inet_pton(AF_INET,"0.0.0.0",&serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid Address\n");
        return -1;
    }

    if(connect(sock, (struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    {
        perror("\nConnection Failed\n");
        return -1;
    }
    printf("\nUspesno povezan na server");
    char poruka[1024];
    while(1)
    {
        printf("\nPosaljite poruku na server:\n");
        fflush(stdin);
        if (fgets(poruka, sizeof(poruka), stdin) != NULL) 
        {
            printf("Saljem: %s", poruka); 
            send(sock, poruka, strlen(poruka), 0);
        }
    }
    printf("\n%s",poruka);
    send(sock,poruka,strlen(poruka),0);
    close(sock);

    return 0;
}