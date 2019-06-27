#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>      // connect()
#include <sys/socket.h>     // connect()
#include <unistd.h>         // close()
#include <netinet/in.h>
#include <errno.h>
#include <pthread.h>

#define SRVPORT 10000
#define CONNECT_NUM 5
#define MAX_NUM 80

typedef struct {

    int socket;
    char buf[MAX_NUM];
    size_t size;

} socket_data;


void *recv_stack(void *arg) {

    socket_data *data = (socket_data *) arg;
    int socket = data->socket;
    char *revBuf = data->buf;
    size_t size = data->size;

    while(1) {
        if (recv(socket, revBuf, size, 0) < 0) {
            printf("recv error\n");
        } else {
            printf("Server: %s\n", revBuf);
            bzero(revBuf, size);
        }
    }
}


void send_stack(int socket, char *sedBuf, size_t size) {
    if (send(socket, sedBuf, size, 0) < 0) {
        printf("send error\n");
    } else {
        printf("Client: %s\n", sedBuf);

        if (0 == strcmp(sedBuf, "quit")) {
            close(socket);
            exit(0);
        }
            
        bzero(sedBuf, size);
    }
}


int main() {


    // declare socket, socketaddr_in
	int clientSock = -1;
	struct sockaddr_in serverAddr;
   

    pthread_t t;


    // initiate socket
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock < 0) {
		printf("fail to create socket\n");
		exit(-1);
	}
	printf("socket create\n");


    // initiate socketaddr_in
	bzero(&serverAddr, sizeof(struct sockaddr_in));
	serverAddr.sin_family = PF_INET;
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
	serverAddr.sin_port = htons((u_short) SRVPORT);


    // connect
	if (connect(clientSock, (struct sockaddr *) &serverAddr, sizeof(struct sockaddr_in)) < 0) {
		printf("fail to connect\n");
		exit(-1);
	}
	printf("connect successful\n");


    // initiate recv
    socket_data *data = malloc(sizeof(socket_data));
    data->socket = clientSock;
    data->size = sizeof(data->buf);
    bzero(data->buf, data->size);


    // start receive
    pthread_create(&t, NULL, recv_stack, (void *)data);


    // initiate sedBuf
	char sedBuf[MAX_NUM] = {0};
    strcpy(sedBuf, "hello server");
    send_stack(clientSock, sedBuf, sizeof(sedBuf));
   

    // main loop
	while (1) {
        scanf("%s", sedBuf);
        send_stack(clientSock, sedBuf, sizeof(sedBuf));
        
        /*
        if (strcmp(sedBuf, "quit")) {
            strcpy(sedBuf, "client leaves");
            send_stack(clientSock, sedBuf, sizeof(sedBuf));
            close(clientSock);
        }
        */
	}
} 
