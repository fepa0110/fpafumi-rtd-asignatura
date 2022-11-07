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

void login_menu(int socketfd, struct sockaddr_in addr)
{
    socklen_t socklen = sizeof(struct sockaddr_in);

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

    int sendTo = sendto(socketfd, &comando, sizeof(comando), 0, (struct sockaddr *)&addr, socklen);
}

void register_menu(int socketfd, struct sockaddr_in addr){
    socklen_t socklen = sizeof(struct sockaddr_in);

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

    int sendTo = sendto(socketfd, &comando, sizeof(comando), 0, (struct sockaddr *)&addr, socklen);
}

void showInitMenu(int socketfd, struct sockaddr_in addr){
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
        login_menu(socketfd, addr);
        break;
    case 2:
        printf("--- Registrarse ---\n");
        register_menu(socketfd, addr);
        break;

    default:
        break;
    }
}

/* char* send_message_menu(){
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[1] = "E";
    char user[11] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese un usuario de destino: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);

    // strcat(comando, user);
    return &user;
    // printf("%s\n", comando);

    // int sendTo = sendto(socketfd, &comando, sizeof(comando), 0, (struct sockaddr *)&addr, socklen);
} */

void nuevo_chat(int socketfd, struct sockaddr_in addr){
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

        int sendTo = sendto(socketfd, &comando, sizeof(comando), 0, (struct sockaddr *)&addr, socklen);
    }

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

void logged_menu(int socketfd, struct sockaddr_in addr){
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
        nuevo_chat(socketfd, addr);
        // login_menu(socketfd, addr);
        break;
    case 2:
        enviar_archivo();
        break;
    case 3:
        break;

    default:
        break;
    }
}

int main(int argc, char *argv[])
{
    

    int socketfd;
    struct sockaddr_in addr;
    socketfd = socket(AF_INET, SOCK_DGRAM, 0);

    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    inet_aton(IP, &(addr.sin_addr));

    int fd;
    char buf[100];

    // printf("Bienvenido cliente\n");
    socklen_t socklen = sizeof(struct sockaddr_in);

    system("clear");
    printf("--- Bienvenido ---\n\n");

    showInitMenu(socketfd, addr);

    // for (;;){
        system("clear");
        printf("Bienvenido!! Que desea hacer?\n\n");
        logged_menu(socketfd, addr);
    // }
    system("clear");
    printf("Hasta luego...");

    close(socketfd);

    exit(EXIT_SUCCESS);
}
