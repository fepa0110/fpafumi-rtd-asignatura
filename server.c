#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/ip.h>

#define PORT 8888
#define IP "127.0.0.1"

int main(int argc, char* argv[]){
	int socketfd;
	struct sockaddr_in addr;

	socketfd = socket(AF_INET, SOCK_DGRAM, 0);

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	inet_aton(IP, &(addr.sin_addr));

	if(bind(socketfd, (struct sockaddr*) &addr, sizeof(addr))==-1){
		perror("Bind");
	}

	// listen(socketfd, 10);

	int fd;
	char buf[100];

	for(;;){
		// socklen_t socklen = sizeof(struct sockaddr_in);
		recv(socketfd, buf, sizeof(buf), 0);
			/* 		if(recv(socketfd, buf, 100, 0) == -1){
						perror("recv");
						exit(1);
					} */
			printf("%s\n", buf);
	}

	close(socketfd);

	exit(EXIT_SUCCESS);
}