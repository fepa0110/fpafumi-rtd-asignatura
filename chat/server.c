/**
 * Servidor Chat UDP
 * -----------------
 *
 * El servidor escucha en el IP:PUERTO indicado como parámetro en la línea de
 * comando, o 127.0.0.1:8888 en caso de que no se indique una dirección.
 *
 * Para terminar la ejecución del servidor, envíar una señal SIGTERM (^C)
 *
 * Se puede probar el funcionamiento del servidor con el programa netcat:
 *
 * nc -u 127.0.0.1 8888
 *
 *
 * ---
 * Autor: Francisco Paez
 * Fecha: 2022-06-03
 * Última modificacion: 2022-06-03
 */
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
#define BUFSIZE 100

// Máximo número de conexiones pendientes en el socket TCP.
#define PENDING 10

#define MAX_USERS 100

// Estructura de datos que almacena información de un usuario.
struct user {
    int id;                     // Identificador númerico único
    char name[10];              // Nombre del usuario
    int status;                 // Online - Offline
    struct sockaddr_in addr;    // Client addr
    int fd;
};
typedef struct user User;

// "Lista" de usuarios registrados
static User *users;

// mutex global
pthread_mutex_t lock;

// Cierra el socket al recibir una señal SIGTERM.
void handler(int signal){
    exit(EXIT_SUCCESS);
}

int user_count(int op){
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

int user_registration(char* username){
    // Busca el primer lugar libre
    int i;
    for (i = 0; i < 100; i++) {
        if (users[i].id == 0) {
            users[i].id = i+1;
            users[i].status = 0;
            strncpy(users[i].name, username, strlen(username));
            return users[i].id;
        }
    }
    return -1;
}

/* int user_login(int id, struct sockaddr_in addr){
    int i;
    for (i = 0; i < 100; i++) {
        if (users[i].id == id) {
            users[i].status = 1;
            users[i].addr = addr;
            printf("[%s:%d]\n", inet_ntoa(users[i].addr.sin_addr), ntohs(users[i].addr.sin_port));
            return users[i].status;
        }
    }
    return -1;
} */

int user_login(int socket){
    printf("user_login");
    User usuario;

    memset(&usuario,0,sizeof(usuario));

    char buf[11];

    int n = recv(socket, buf, sizeof(buf),0);

    return 0;

}

static void* handleHilo(void *arg)
{       
    int socket = *((int *) arg);

    int n; 

    char comando[2];
    memset(&comando, 0, sizeof(comando));

    n = recv(socket, comando, 1, 0);
    perror("recv hilo");
    printf("n: %d\n",n);

    comando[1] = '\0';

    printf("Comando: %s\n",comando);
        
    if(strcmp(comando,"L\0") == 0){        
        printf("Inicio sesion");
        // int id = handleLoguearUsuario(sock);
        int id = user_login(socket);
        
        /* if(id > 0){
            atenderSesion(sock, id);
        }else{
            printf("sesion no iniciada, error [atenderSesion:%d]\n",id);
        } */
    }
        
    pthread_exit(NULL);
}

int main(int argc, char* argv[]){
    pthread_t hilo;
    
    if(pthread_mutex_init(&lock, NULL) <0){
        exit(EXIT_FAILURE);
    }

    users = malloc(sizeof(User) * MAX_USERS);

    struct sockaddr_in addr;

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
    
    char buf[BUFSIZE];
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

        // Existe una conexión entrante en el socket TCP.
        /*if (FD_ISSET(socket_tcp, &read_fds)) {
            int n;
            sock = accept(socket_tcp, (struct sockaddr*) &src_addr, &src_addr_len);
            if (sock == -1) {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            pid_t pid = fork();
            switch (pid) {
                case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                case 0:
                    while ((n = recv(sock, buf, BUFSIZE,0)) > 0) {
                        if (n == -1) {
                            perror("recv");
                            exit(EXIT_FAILURE);
                        }
                        // Elimina '\n' al final del buffer.
                        buf[n-1] = '\0';
                        printf("[%s:%d] %s\n", inet_ntoa(src_addr.sin_addr), ntohs(src_addr.sin_port), buf);

                        command = buf[0];

                        // Ejecuta el comando enviado por el cliente.
                        switch(command) {
                            case 'R':
                                sprintf(buf, "%d\n", user_registration(&(buf[1])));
                                break;
                            case 'L':
                                sprintf(buf, "%d\n", user_login(atoi(&buf[1]), src_addr));
                                break;
                            case 'Q':
                                sprintf(buf, "%d\n", user_count(atoi(&buf[1])));
                                break;
                            case 'S':
                                dest = users[atoi(&buf[1])-1];
                                sprintf(buf, "%s\n", &buf[2]);
                                n = sendto(socket_tcp, buf, strlen(&buf[2])+1, 0, (struct sockaddr*) &(dest.addr), src_addr_len);
                                sprintf(buf, "%ld\n", n);
                                break;
                            default:
                                sprintf(buf, "E\n");
                        }

                        // Envía eco
                        buf[n-1] = '\n';
                        write(sock, buf, strlen(buf));
                    }
                    close(sock);
                    exit(EXIT_SUCCESS);
                default:
                    break;
            }
        }*/