#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

#define SRVPORT 10000
#define CONNECT_NUM 5
#define MAX_NUM 80


typedef struct {

    int socket;
    char buf[MAX_NUM];
    size_t size;
    int count;

} socket_data;


void *recv_stack(void* arg) {

    socket_data *data = (socket_data *)arg;
    int socket = data->socket;
    char *buf = data->buf;
    size_t size = data->size;
    int count = data->count;

    
    while(1) {
        if (recv(socket, buf, size, 0) < 0) {
            printf("read error\n");
        } 
        else {

            printf("Client_%d: %s\n", count, buf);

            if (0 == strcmp(buf, "quit")) {
                pthread_exit(0);
            }

            bzero(buf, size);
        }
    }
}



void send_stack(int socket, char *buf, size_t size) {
    if (send(socket, buf, size, 0) == -1) {
        printf("send error\n");
    } 
    else {
        printf("Server: %s\n", buf);
        bzero(buf, size);
    }
}


int main() {
   
    // ===================
    int serverSock = -1, clientSock = -1;
    struct sockaddr_in serverAddr, clientAddr;
    int addrlen = sizeof(clientAddr);


    pthread_t t;
    int count = 0;
    

    // ===================
    
    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSock < 0) {
        printf("fail to create serverSock\n");
        exit(-1);
    }
    printf("serverSock create\n");

    // 初始化 ============
   
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    /**serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");*/
    serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
    serverAddr.sin_port = htons((u_short) SRVPORT);

    // bind ==============

    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(struct sockaddr_in)) == -1) {
        printf("Bind error\n");
        exit(-1);
    }
    printf("Bind successful\n");

    // listen ============

    if (listen(serverSock, 10) == -1) {
        printf("Listen error\n");
    }
    printf("Start to listen\n");

    // init buff =========
    char sedBuf[MAX_NUM] = {0};

    // main loop =========
    // start server
    while (1) {

        // get a client
        clientSock = accept(serverSock, NULL, NULL);
    
    
        // initiate socket_data
        socket_data *data = malloc(sizeof(socket_data));
        data->socket = clientSock;
        data->size = sizeof(data->buf);
        bzero(data->buf, data->size);
        data->count = ++count;
        sprintf(sedBuf, "welcome, client_%d", count);


        // send welcome message
        send_stack(clientSock, sedBuf, sizeof(sedBuf));


        pthread_create(&t, 0, recv_stack, (void *)data);

    }
    return 0;
} 
