#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <signal.h>
#include <pthread.h>

// Dirección por defecto del servidor.
#define PORT 8888
#define IP   "127.0.0.1"

// Tamaño del buffer en donde se reciben los mensajes.
#define BUFFER_SIZE 25

// Máximo número de conexiones pendientes en el socket TCP.
#define PENDING 10

#define MAX_USERS 100

#define CODIGO_FALLIDO -1

#define CODIGO_EXITOSO 0

// Estructura de datos que almacena información de un usuario.
struct user {
    int id;                     // Identificador númerico único
    char name[10];              // Nickname del usuario
    int status;                 // Online - Offline
    int fd;
    char buf[100];
};
typedef struct user User;

// "Lista" de usuarios registrados
static User *users;

int cantidadRegistrados;

// mutex global
pthread_mutex_t lock;

// Cierra el socket al recibir una señal SIGTERM.
void handler(int signal){
    exit(EXIT_SUCCESS);
}

int userCount(int op){
    int i, c = 0;
    switch(op) {
        case 0: // Cuenta todos los usuarios registrados
            for (i = 0; i < 100; i++) {
                if (users[i].id > 0) {
                    c = c + 1;
                }
            }
            return c;
        case 1: // Cuenta los usuario actualmente conectados
            for (i = 0; i < 100; i++) {
                if (users[i].id > 0 && users[i].status == 1) {
                    c = c + 1;
                }
            }
            return c;
        default:
            return -1;
    }
}

int userRegistration(User usuario){
    pthread_mutex_lock(&lock);

    // Busca el primer lugar libre
    int i;

    int resultado = CODIGO_FALLIDO;

    for (i = 0; i < 100; i++) {
        if (users[i].id == 0) {
            users[i].id = i+1;
            users[i].status = 0;
            strncpy(users[i].name, usuario.name, strlen(usuario.name));
            users[i].fd = usuario.fd;
            resultado = users[i].id;
        }
    }

    pthread_mutex_unlock(&lock);

    return resultado;
}

int handleRegisterHilo(int socket){
    printf("userRegistration\n");
    User usuario;
    usuario.fd = socket;

    int resultado = CODIGO_FALLIDO;

    memset(&usuario,0,sizeof(usuario));

    char buf[10];

    int n = recv(socket, buf, sizeof(buf),0);

    printf("%s\n",&buf);

    strncpy(usuario.name,buf,10);
    
    resultado = userRegistration(usuario);

    if(resultado == CODIGO_FALLIDO){
        printf("Fallo registro\n");
    }
    else{
        printf("Usuario registrado\n");
        send(socket,"Registrado",sizeof("Registrado"),0);
    }

    return resultado;

}

int userLogin(User usuario){
    pthread_mutex_lock(&lock);

    int i;
    int resultado = CODIGO_FALLIDO;

    for (i = 0; i < 100; i++) {
        if (strcmp(users[i].name,usuario.name) == 0) {
            users[i].status = 1;
            users[i].fd = usuario.fd;
            printf("[%s:%d]\n", users[i].name ,users[i].id);
            resultado = i;
        }
    }

    pthread_mutex_unlock(&lock);

    return resultado;
}

// Buscar usuario por username
int buscarUsuario(User usuario){
    pthread_mutex_lock(&lock);

    printf("BuscarUsuario");

    int indiceUsuario = CODIGO_FALLIDO;
    int i;

    for(i=0; i < cantidadRegistrados; i++){
        printf("for");

        if(strncmp(users[i].name, usuario.name, 10) == 0){
            printf("Lo encontre!!");
            return i;
        }
    }

    printf("Fin BuscarUsuario");

    pthread_mutex_unlock(&lock);

    return indiceUsuario;


}

void sendMessage(int socket, int indiceUsuarioLogueado, User usuarioDestino){
    // pthread_mutex_lock(&lock);
    char mensaje[25];

    User usuarioLogueado = users[indiceUsuarioLogueado];
    // Leo el mensaje enviado
    int leido = recv(socket, mensaje, sizeof(mensaje),0);

    printf("Mensaje: %s",mensaje);

    //Envio el mensaje recibido al destino
    sprintf(usuarioDestino.buf, "[%s] %s",usuarioLogueado.name,mensaje);
    // int mensajeEnviado = send(usuario.fd, mensaje, sizeof(mensaje),0);
    printf("Mensaje enviado");
    // pthread_mutex_unlock(&lock);

}

void handleSendMessage(int socket, int indiceUsuarioLogueado){

    User usuarioDestino;

    memset(&usuarioDestino, 0, sizeof(usuarioDestino));
    
    char usernameLeido[10]; 

    int indiceUDestino = CODIGO_FALLIDO;

    printf("Entre\n");
    int n = recv(socket, usernameLeido, sizeof(usernameLeido), 0);
    
    printf("User: %s\n",usernameLeido);

    memcpy(usuarioDestino.name,usernameLeido, sizeof(usernameLeido));

    indiceUDestino = buscarUsuario(usuarioDestino);
    // int indiceUsuarioDestino = 1;
    // printf("indice: %d\n",indiceUsuarioDestino);
    
    if(indiceUDestino == CODIGO_FALLIDO){
        n = send(socket,"-1",sizeof("-1"),0);
        printf("Usuario no encontrado");
    }
    else{
        n = send(socket,"0",sizeof("0"),0);
        printf("Usuario encontrado");

        usuarioDestino = users[indiceUDestino];
        sendMessage(socket, indiceUsuarioLogueado, usuarioDestino);
    }

    printf("Chat finalizado");

}

int handleBuscarUsuario(int socket){
    char username[10];

    int n = recv(socket, username, sizeof(username),0);

    User usuarioBuscado;
    // usuarioBuscado.name = username;

    memcpy(usuarioBuscado.name,username, sizeof(username));

    usuarioBuscado = users[buscarUsuario(usuarioBuscado)];

    n = send(socket, usuarioBuscado.name, sizeof(usuarioBuscado.id), 0);
}

void handleVerMensajes(int socket, int indiceUsuario){
    User usuario = users[indiceUsuario];

    int mensajesEnviados = send(socket, usuario.buf, sizeof(usuario.buf),0);

    printf("Mensajes mostrados...");
}

// Metodo para manejar el usuario logueado
void handleLoggedUser(int socket, int indiceUsuario){
    char comando[2];
    memset(&comando, 0, sizeof(comando));

    int codigo;
    int n = recv(socket, comando, 1, 0);
    perror("recv hilo");
    printf("n: %d\n",n);

    comando[1] = '\0';

    printf("Comando: %s\n",comando);
        
    if(strcmp(comando,"E\0") == 0){        
        printf("Iniciando chat\n");
        handleSendMessage(socket, indiceUsuario);
    }
    else if(strcmp(comando,"V\0") == 0){        
        printf("Mostrando mensajes\n");
        handleVerMensajes(socket, indiceUsuario);
    }
    else if(strcmp(comando,"B\0") == 0){
        codigo = handleBuscarUsuario(socket);
    }
    
}

int handleLoginHilo(int socket){
    printf("userLogin\n");
    User usuario;
    usuario.fd = socket;

    memset(&usuario,0,sizeof(usuario));

    char buf[10];

    int n = recv(socket, buf, sizeof(buf),0);

    printf("%s",&buf);

    strncpy(usuario.name,buf,10);
    
    int indiceLogin = userLogin(usuario);

    if(indiceLogin > CODIGO_FALLIDO){
        printf("Usuario logueado\n");

        // Envio login exitoso
        send(socket, "0",sizeof("0"),0);
        return indiceLogin;
    }

    printf("Usuario no logueado\n");
    send(socket, "-1",sizeof("-1"),0);

    return CODIGO_FALLIDO;
}

static void* handleHilo(void *arg){       
    int socket = *((int *) arg);

    int n; 

    int indiceLogin;

    char comando[2];
    memset(&comando, 0, sizeof(comando));

    n = recv(socket, comando, 1, 0);
    perror("recv hilo");
    printf("n: %d\n",n);

    comando[1] = '\0';

    printf("Comando: %s\n",comando);
        
    if(strcmp(comando,"L\0") == 0){        
        printf("Inicio sesion\n");
        indiceLogin = handleLoginHilo(socket);
    }
    else if (strcmp(comando,"R\0") == 0)
    {
        printf("Registro");
        indiceLogin = handleRegisterHilo(socket);
    }

    if(indiceLogin > CODIGO_FALLIDO){
        handleLoggedUser(socket, indiceLogin);
    }
    // else??

    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    pthread_t hilo;
    
    if(pthread_mutex_init(&lock, NULL) <0){
        exit(EXIT_FAILURE);
    }

    users = malloc(sizeof(User) * MAX_USERS);

    struct sockaddr_in addr;

    users[0].id = 1;
    strcpy(users[0].name,"fb");
    strcpy(users[0].buf,"Hola\n");
    users[0].status = 0;

    users[1].id = 2;
    strcpy(users[1].name,"aa");
    users[1].status = 0;

    cantidadRegistrados = 2;

    // Descriptor de archivo del socket.
    int socket_tcp;
    // Configura el manejador de señal SIGTERM.
    signal(SIGTERM, handler);

    // Crea el socket.
    socket_tcp = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_tcp == -1) {
        perror("socket_tcp");
        exit(EXIT_FAILURE);
    }

    // Estructura con la dirección donde escuchará el servidor.
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    if (argc == 3) {
        addr.sin_port = htons((uint16_t) atoi(argv[2]));
        inet_aton(argv[1], &(addr.sin_addr));
    } else {
        addr.sin_port = htons(PORT);
        inet_aton(IP, &(addr.sin_addr));
    }

    // Permite reutilizar la dirección que se asociará al socket.
    int optval = 1;
    int optname = SO_REUSEADDR | SO_REUSEPORT;
    if(setsockopt(socket_tcp, SOL_SOCKET, optname, &optval, sizeof(optval)) == -1) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // Asocia el socket con la dirección indicada. Tradicionalmente esta 
    // operación se conoce como "asignar un nombre al socket".
    int b;
    b = bind(socket_tcp, (struct sockaddr*) &addr, sizeof(addr));
    if (b == -1) {
        perror("bind tcp");
        exit(EXIT_FAILURE);
    }

    listen(socket_tcp, PENDING);

    printf("Escuchando en %s:%d ...\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    
    char buf[BUFFER_SIZE];
    struct sockaddr_in src_addr;
    socklen_t src_addr_len;
    fd_set read_fds;

    char command;
    User dest;

    FD_ZERO(&read_fds);
    
    for(;;) {
        // Agrega el socketTCP en el set de descriptores de archivos
        // que se monitorearan por medio de select().
        FD_SET(socket_tcp, &read_fds);

        int s = select(socket_tcp + 1, &read_fds, NULL, NULL, NULL);
        if (s == -1) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        memset(&src_addr, 0, sizeof(struct sockaddr_in));
        src_addr_len = sizeof(struct sockaddr_in);
        int sock;

        if (FD_ISSET(socket_tcp, &read_fds)){
            sock = accept(socket_tcp, (struct sockaddr *)&src_addr, &src_addr_len);
            printf("socket accept %d\n", sock);

            pthread_create(&hilo,NULL,&handleHilo,(void*) &sock);
            
        }


    }
    
    if ( pthread_join(hilo, NULL) != 0) {
        perror("pthread_join");
    }

    close(socket_tcp);
}