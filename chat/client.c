#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define PORT 8888
#define IP "127.0.0.1"

void login_menu(int socketfd, struct sockaddr_in addr){
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[11] = "L";
    char user[10] = " ";

    while(strcmp(user," ") == 0){
        printf("Ingrese su usuario: ");
        scanf("%s",&user);
    }
    printf("%s\n",user);

    // strcpy("L", &buf);
    strcat(comando, user);
    printf("%s\n",comando);

    int sendTo = sendto(socketfd, &comando, sizeof(comando), 0, (struct sockaddr *)&addr, socklen);
}

void showInitMenu(int socketfd, struct sockaddr_in addr)
{
    printf("1. Login\n");
    printf("2. Registrarse\n");

    // bool pedirOpcion = true;
    int command = 0;

    while(command == 0){
        printf("Ingrese una opcion: ");
        scanf("%d",&command);

        // if(command != '0') pedirOpcion = false;
    }
    printf("%d\n",command);

    switch(command)
    {
        case 1:
            printf("--- Login ---\n");
            login_menu(socketfd, addr);
            break;
        case 2:
            printf("Registrarse\n");
            break;

        default:
            break;
    }
}

int main(int argc, char *argv[]){
    int socketfd;
    struct sockaddr_in addr;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_aton(IP, &(addr.sin_addr));

    int fd;
    char buf[100];


    for (;;){
        printf("Bienvenido cliente\n");
        socklen_t socklen = sizeof(struct sockaddr_in);

        // scanf("%s",&buf);
        // printf("%s\n",buf);

        showInitMenu(socketfd, addr);

        // sendto(socketfd, &buf, 2, 0, (struct sockaddr *)&addr, socklen);
        // printf("Mensaje enviado\n");
        // sleep();
    }

    close(socketfd);

    exit(EXIT_SUCCESS);
}
