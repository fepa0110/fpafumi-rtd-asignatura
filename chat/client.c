#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define SA struct sockaddr
#define PORT 8888
#define IP "127.0.0.1"

void login_menu(int socketfd)
{
    // socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[11] = "L";
    char user[10] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese su usuario: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);

    strcat(comando, user);
    // printf("%s\n", comando);

    int sendTo = send(socketfd, &comando, sizeof(comando), 0);
}

void register_menu(int socketfd){
    // socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[11] = "R";
    char user[10] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese un nombre de usuario: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);

    strcat(comando, user);
    // printf("%s\n", comando);

    int sendTo = send(socketfd, &comando, sizeof(comando), 0);
}

void showInitMenu(int socketfd){
    printf("1. Login\n");
    printf("2. Registrarse\n");

    int opcion = 0;

    while (opcion == 0)
    {
        printf("\nIngrese una opcion: ");
        scanf("%d", &opcion);
    }
    printf("%d\n", opcion);

    system("clear");

    switch (opcion)
    {
    case 1:
        printf("--- Login ---\n");
        login_menu(socketfd);
        break;
    case 2:
        printf("--- Registrarse ---\n");
        register_menu(socketfd);
        break;

    default:
        break;
    }
}

void nuevo_chat(int socketfd){
    system("clear");
    
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[36] = "E";
    char user[10] = " ";
    bool salir = false; 
    char mensaje[25];

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese un usuario con quien deseas chatear: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);
    strcat(comando,user);

    system("clear");
    printf("%s","Escribe \"salir\" si deseas abandonar el chat\n");

    while (!salir)
    {
        scanf("%s",&mensaje);
        
        if(strcmp(mensaje, "salir") == 0){
            salir = true;
        }

        strcat(comando,mensaje);

        int sendTo = send(socketfd, &comando, sizeof(comando), 0);
        // int sendt = send(socketfd, &comando, sizeof(comando), 0);
        // int sendt = write(socketfd, &comando, sizeof(comando));
    }

    system("clear");

    // strcat(comando, user);
    // return &user;
    // printf("%s\n", comando);

}

void enviar_archivo(){
    system("clear");
    
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[1] = "F";
    char user[10] = " ";
    // bool salir = false; 
    // char mensaje[25];

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese un usuario a quien deseas enviar el archivo: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);
}

void logged_menu(int socketfd){
    bool salir = false;
    while (!salir){
        printf("1. Enviar mensaje\n");
        printf("2. Enviar archivo\n");
        printf("3. Salir\n");
    
        int opcion = 0;

        while (opcion == 0){
            printf("\nIngrese una opcion: ");
            scanf("%d", &opcion);
        }
        printf("%d\n", opcion);

        switch (opcion){
        case 1:
            // printf("--- Enviar mensaje ---\n");
            nuevo_chat(socketfd);
            // login_menu(socketfd, addr);
            break;
        case 2:
            enviar_archivo();
            break;
        case 3:
            salir = true;
            break;

        default:
            break;
        }
    }

}

int main(int argc, char *argv[])
{
    int socketfd;
    struct sockaddr_in addr;
    socketfd = socket(AF_INET, SOCK_STREAM, 0);

    if (socketfd == -1)
    {
        printf("socket creation failed...\n");
        exit(0);
    }
    bzero(&addr, sizeof(addr));

    // assign IP, PORT
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(IP);
    addr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(socketfd, (SA *)&addr, sizeof(addr)) != 0)
    {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    // memset(&addr, 0, sizeof(struct sockaddr_in));
    // addr.sin_family = AF_INET;
    // addr.sin_port = htons(PORT);
    // inet_aton(IP, &(addr.sin_addr));

    char buf[100];

    // printf("Bienvenido cliente\n");
    // socklen_t socklen = sizeof(struct sockaddr_in);

    // system("clear");
    printf("--- Bienvenido ---\n\n");

    showInitMenu(socketfd);

    // for (;;){
        // system("clear");
        printf("Bienvenido!! Que desea hacer?\n\n");
        logged_menu(socketfd);
    // }
    system("clear");
    printf("Hasta luego...");

    close(socketfd);

    // exit(EXIT_SUCCESS);
}
