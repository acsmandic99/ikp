#include "data_destination.h"
#include <stdio.h>

int main()
{
    int port;
    printf("\nEnter port for data destination: ");
    scanf("%d",&port);
    create_listener(port);
    return 0;
}