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

#define CODIGO_FALLIDO 0

#define CODIGO_EXITOSO 1

void vaciarStdin(){
    while (getchar() != '\n');
}

int login_menu(int socketfd)
{
    // socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[11] = "L";
    char user[10] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese su usuario: ");
        scanf("%s", &user);
    }
    // printf("%s\n", user);

    strcat(comando, user);
    // printf("%s\n", comando);

    int sendTo = send(socketfd, &comando, sizeof(comando), 0);

    char mensajeLeido[1];

    int recibido = recv(socketfd, mensajeLeido, sizeof(mensajeLeido),0);
    printf("%s\n",mensajeLeido);

    vaciarStdin();
    
    return atoi(mensajeLeido);
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

    char mensajeLeido[25];

    int recibido = recv(socketfd, mensajeLeido, sizeof(mensajeLeido),0);
    printf("%s",mensajeLeido);
}

int showInitMenu(int socketfd){
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
        return login_menu(socketfd);
        break;
    case 2:
        printf("--- Registrarse ---\n");
        register_menu(socketfd);
        break;

    default:
        break;
    }

    return 1;
}

void nuevo_chat(int socketfd){
    system("clear");
    
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[11] = "E";
    char user[10] = " ";
    bool salir = false; 
    char mensaje[25];
    char respuesta[1];

    // vaciarStdin();

    while (strcmp(user, " ") == 0){
        printf("Ingrese un usuario con quien deseas chatear: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);
    strcat(comando,user);

    int sendUser = send(socketfd, &comando, sizeof(comando), 0);

    // system("clear");
    // printf("%s","Escribe \"salir\" si deseas abandonar el chat\n");

    int recvRespuesta = recv(socketfd,respuesta,sizeof(respuesta),0);
    printf("respuesta: %s",respuesta);
    // while (!salir)
    // {
    // system("clear");
    
    if(atoi(respuesta) == 1){
        scanf("Ingrese un mensaje: %s",&mensaje);
        int mensajeEnviado = send(socketfd, &comando, sizeof(comando), 0);
        printf("--- Mensaje enviado: %d ---\n\n",mensajeEnviado);
    }
    else printf("--- Hubo un error al enviar el mensaje ---\n\n");
        // strcat(comando,mensaje);
        
/*         if(strcmp(mensaje, "salir") == 0){
            // vaciarStdin();
            send(socketfd,"X\0",sizeof("X\0"),0);
            salir = true;
        } */


    // }

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

void buscar_usuario(int socketfd){
    system("clear");
    
    char comando[1] = "B";
    char user[10] = " ";

    char buf[12];
    char infoUsuario[50];
    // bool salir = false; 
    // char mensaje[25];

    while (strcmp(user, " ") == 0)
    {
        printf("Ingresa el usuario que deseas buscar: ");
        scanf("%s", &user);
    }

    printf("%s\n", user);

    sprintf(buf,"B%s\0",user);

    printf("Buffer: %s",buf);

    int enviado = send(socketfd,buf,sizeof(buf),0);

    int recibido = recv(socketfd, infoUsuario, sizeof(infoUsuario),0);

    printf("%s\n",infoUsuario);
}

void logged_menu(int socketfd){
    bool salir = false;
    while (!salir){
        printf("1. Enviar mensaje\n");
        printf("2. Enviar archivo\n");
        printf("3. Buscar usuario\n");
        printf("4. Salir\n");
    
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
            buscar_usuario(socketfd);
            break;
        case 4:
            salir = true;
            break;

        default:
            break;
        }
    }
    
    exit(EXIT_SUCCESS);

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

    system("clear");
    printf("--- Bienvenido ---\n\n");

    if(showInitMenu(socketfd) == 1){
        printf("Bienvenido!! Que desea hacer?\n\n");
        logged_menu(socketfd);
    }

    // system("clear");
    printf("Hasta luego...");

    close(socketfd);

    // exit(EXIT_SUCCESS);
}
