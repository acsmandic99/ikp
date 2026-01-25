#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
#include "../messages/messages.h"

void create_listener(int PORT)
{
    struct sockaddr_in address;
    int data_destination_fd,new_socket;
    int addrlen = sizeof(address);

    if((data_destination_fd = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP))== 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    printf("\nData destination created\nListeneing on:%s:%d\n",inet_ntoa(address.sin_addr),PORT);

    if(bind(data_destination_fd,(struct sockaddr*)&address,sizeof(address)) < 0)
    {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if(listen(data_destination_fd,1) == -1)
    {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in client_address;
    new_socket = accept(data_destination_fd, (struct sockaddr *)&client_address,  (socklen_t*)&addrlen);

    if(new_socket < 0)
    {
        perror("Accecpt failed");
        exit(EXIT_FAILURE);
    }
    while(1)
    {
        Request msg;
        int val_read = read(new_socket,&msg,sizeof(Request));
        if(val_read == 0)
        {
            printf("\nCentral agregator has disconnected unexpectedly...");
            printf("\nShutting down...");
            exit(EXIT_FAILURE);
        }

        if(msg.request_type == REQUEST_POWER)
        {
            printf("\nCentral agregator requests more power\n");
            printf("\nHe needs minimum of: %f kW\n",msg.power_amount);
            printf("Approve more power? Y/N\n");
            char approve;
            scanf(" %c",&approve);
            Response response;
            if(approve == 'Y')
            {
                printf("\nType in the amount of power to give to central agregator: ");
                float give_power;
                scanf("%f",&give_power);
                getchar();
                response.response_type = APPROVED;
                response.power_amount = give_power;
            }else {
                printf("\nRequest was REJECTED.\n");
                response.response_type = REJECTED;
                response.power_amount = 0;
            }
            
            write(new_socket,&response,sizeof(response));
        }
        else {
            printf("\nCentral agregator has excess power\n");
            printf("\nHe returns %f kW\n",msg.power_amount);
        }
    }
}