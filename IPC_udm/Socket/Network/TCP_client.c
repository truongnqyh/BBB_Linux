#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <stdio.h>

#define SERVER_IP "127.1.1.1"
#define SERVER_PORT 2000
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32

int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};

typedef struct data{
    int num1;
    int num2;
}data_to_server_t;

typedef struct response{
    int result;
}response_back_to_client;

int main (int argc, char *argv[]){
    struct sockaddr_in client_addr;
    int data_socket;
    int ret_val;
    int index;
    int input;
    socklen_t addrlen;
    char buffer[BUFFER_SIZE];
    data_to_server_t data_send;
    response_back_to_client response;

    /* Create data socket */
    data_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if(data_socket == -1){
        printf("Create data socket fail\n");
        exit(EXIT_FAILURE);
    }
    memset(&client_addr, 0, sizeof(struct sockaddr_in));

    /* Connect socket to socket address */
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = SERVER_PORT;
    struct hostent *host = (struct hostent *)gethostbyname(SERVER_IP);
    client_addr.sin_addr = *((struct in_addr *)host->h_addr);
    ret_val = connect(data_socket, (struct sockaddr *)&client_addr, sizeof(client_addr));
    if (ret_val == -1) {
        perror("connect fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Connect data socket successfully\n");
    addrlen = sizeof(struct sockaddr);
    while(1)
    {
        printf("Enter data 1 to send\n");
        scanf("%d", &data_send.num1);
        printf("Enter data 2 to send\n");
        scanf("%d", &data_send.num2);   
        ret_val = sendto(data_socket, &data_send, sizeof(data_send), 0, (struct sockaddr *)&client_addr, sizeof(client_addr));
        printf("Number of bytes sent: %d, data1: %d, data2: %d\n", ret_val, data_send.num1, data_send.num2);
        if((data_send.num1 == 0) && (data_send.num2 == 0)){
            printf("Client exit\n");
            break;
        }
        ret_val = recvfrom(data_socket, &response, sizeof(response), 0, (struct sockaddr *)&client_addr, &addrlen);
        printf("Number of bytes received: %d\n", ret_val);
        printf("Result: %d\n", response.result);
    }

    return 0;
}