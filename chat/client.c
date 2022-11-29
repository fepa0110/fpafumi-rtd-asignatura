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

#define CODIGO_FALLIDO -1

#define CODIGO_EXITOSO 0

void vaciarStdin(){
    while (getchar() != '\n');
}

int loginMenu(int socketfd)
{
    char comando[11] = "L";
    char user[10] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese su usuario: ");
        scanf("%s", &user);
    }

    strcat(comando, user);

    int sendTo = send(socketfd, &comando, sizeof(comando), 0);

    char mensajeLeido[2];

    int recibido = recv(socketfd, mensajeLeido, sizeof(mensajeLeido),0);
    // printf("%s\n",mensajeLeido);

    vaciarStdin();
    
    return atoi(mensajeLeido);
}

void registerMenu(int socketfd){
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
        return loginMenu(socketfd);
        break;
    case 2:
        printf("--- Registrarse ---\n");
        registerMenu(socketfd);
        break;

    default:
        break;
    }

    system("clear");

    return CODIGO_FALLIDO;
}

void nuevoChat(int socketfd){
    system("clear");
    
    char comando[11] = "E";
    char user[10] = " ";
    bool salir = false; 
    char mensaje[25];
    char respuesta[3];

    // vaciarStdin();

    // printf("sieze:%d\n",sizeof("-1"));
    while (strcmp(user, " ") == 0){
        printf("Ingrese un usuario con quien deseas chatear: ");
        scanf("%s", &user);
    }
    printf("usuario: %s\n", user);
    strcat(comando,user);

    // Envio el comando E<username>
    int sendUser = send(socketfd, &comando, sizeof(comando), 0);

    // Obtengo la respuesta del servidor
    int recvRespuesta = recv(socketfd,respuesta,sizeof(respuesta),0);
    printf("respuesta: %s\n",respuesta);

    // Si se encontro el usuario ingresado
    if(atoi(respuesta) == CODIGO_EXITOSO){
        vaciarStdin();

        printf("Ingrese un mensaje: ");
        scanf("%s",&mensaje);

        int mensajeEnviado = send(socketfd, &mensaje, sizeof(mensaje), 0);
        printf("--- Mensaje enviado: %d ---\n\n",mensajeEnviado);
    }
    else printf("--- Hubo un error al enviar el mensaje ---\n\n");

}

void enviarArchivo(){
    system("clear");
    
    printf("Caracteristica aun no implementada\n\n");
    socklen_t socklen = sizeof(struct sockaddr_in);

    char comando[1] = "F";
    char user[10] = " ";

    while (strcmp(user, " ") == 0)
    {
        printf("Ingrese un usuario a quien deseas enviar el archivo: ");
        scanf("%s", &user);
    }
    printf("%s\n", user);
}

void buscarUsuario(int socketfd){
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

void verMensajes(int socketfd){
    system("clear");

    int enviado = send(socketfd, "V",sizeof("V"),0);

    char mensajes[100];
    char salir[10] = " ";
    int recibido = recv(socketfd, mensajes, sizeof(mensajes), 0);

    printf("--Mis mensajes--\n\n");

    printf("%s\n",mensajes);
    printf("Escribi \"salir\" para volver atras\n");

    do{
        scanf("%s",&salir);
        // printf("\n");
    }
    while(strcmp(salir,"salir") != 0);

    system("clear");
}

void loggedMenu(int socketfd){
    bool salir = false;

    while (!salir){
        printf("---Menu principal---\n\n");
        printf("1. Enviar mensaje\n");
        printf("2. Ver mis mensajes\n");
        printf("3. Enviar archivo\n");
        printf("4. Buscar usuario\n");
        printf("5. Salir\n");
    
        int opcion = 0;

        while (opcion == 0){
            printf("\nIngrese una opcion: ");
            scanf("%d", &opcion);
        }
        printf("%d\n", opcion);

        switch (opcion){
        case 1:
            // printf("--- Enviar mensaje ---\n");
            nuevoChat(socketfd);
            // loginMenu(socketfd, addr);
            break;
        case 2:
            verMensajes(socketfd);
            break;
        case 3:
            enviarArchivo();
            break;
        case 4:
            buscarUsuario(socketfd);
            break;
        case 5:
            salir = true;
            break;

        default:
            break;
        }
    }

    system("clear");
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
        printf("No se pudo conectar al servidor...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");

    char buf[100];

    system("clear");
    printf("--- Bienvenido ---\n\n");

    if(showInitMenu(socketfd) == CODIGO_EXITOSO){
        system("clear");

        printf("Bienvenido!! Que desea hacer?\n\n");
        loggedMenu(socketfd);
    }
    else{
        printf("Usuario no encontrado");
    }

    printf("\nHasta luego...");

    close(socketfd);

    exit(EXIT_SUCCESS);
}
