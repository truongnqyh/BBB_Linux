/* 
inet_ntoa(addr)
ntohs(port)
recvfrom
sento
struct hosten* host = (struct hostent*)gethostbyname("localhost");
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#define SERVER_PORT 2000
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32

typedef struct data{
    int num1;
    int num2;
}data_to_server_t;

typedef struct response{
    int result;
}response_back_to_client;

int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};


static void init_monitor_fd_set(){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        monitor_fd_set[index] = -1; 
    }
}

static void add_to_monitor_fd_set(int fd){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        if(monitor_fd_set[index] == -1){
            monitor_fd_set[index] = fd;
            break;
        }
    }
}

static void remove_from_monitor_fd_set(int fd){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        if(monitor_fd_set[index] == fd){
            monitor_fd_set[index] = -1;
            break;
        }
    }
}

static void clone_to_fd_set(fd_set *fd_set){
    int index = 0;

    FD_ZERO(fd_set);
    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
        if(monitor_fd_set[index] != -1){
            FD_SET(monitor_fd_set[index], fd_set);
        }
    }

}

static int max_fd(){
    int index = 0;
    int max = 0;

    max = -1;
    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
        if(monitor_fd_set[index] > max){
            max = monitor_fd_set[index];
        }
    }

    return max;
}

int main (int argc, char *argv[]){
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    int master_socket_fd;
    int communication_socket_fd;
    int ret_val;
    fd_set read_fd;
    int index;
    int temp_fd;
    int temp_data;
    char buffer[BUFFER_SIZE];
    socklen_t addrlen;
    response_back_to_client response;

    /* Create master socket */
    /** -->> SOCK_DGRAM for datagram based communication ***/
    master_socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(master_socket_fd == -1){
        perror("Create master socket fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Create master socket successfully\n");
    /* Initialize server address with value 0 */
    memset(&server_addr, 0, sizeof(server_addr));
    /* Add properties for sockaddr_ins */
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = SERVER_PORT;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    /* Bind master socket to server address */
    ret_val = bind(master_socket_fd, (struct sockaddr *) &server_addr, (socklen_t)sizeof(server_addr));
    if(ret_val == -1){
        perror("bind fail\n");
        exit(EXIT_FAILURE);
    }
    printf("bind successfully\n");
    /* Listen() call -> maintain the max length of queue */
    /* The backlog size is set to 20 then while one request is being 
    processed, the next request can be waiting */
    ret_val = listen(master_socket_fd, MAX_CLIENTS_SUPPORT);
    if(ret_val == -1){
        perror("bind fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Listen successfully\n");
    /* Init monitor fd set with value -1 */
    init_monitor_fd_set();
    /* Add master socket to monitor fd set */
    add_to_monitor_fd_set(master_socket_fd);
    addrlen = sizeof(struct sockaddr);
    /* Server work 24*7 then shoule be put in an infinite loop */
    while(1){
        /* Clone to fd_set type to be an argument for select() */
        clone_to_fd_set(&read_fd);
        /* Waiting select() to be unblocked */
        printf("Waiting on select() sys call\n");
        /* Call select() to wait for incoming connections, in blocking state,
        the connection initiation request or data requests 
        arrives on any of the file Drscriptors in the 'read_fd' set */
        if(select(max_fd() + 1, &read_fd, NULL, NULL, NULL) == -1){
            perror("select fail\n");
            exit(EXIT_FAILURE);
        }
        /* If it's the master socket */
        if(FD_ISSET(master_socket_fd, &read_fd)){
            printf("New connection request\n");
            /* Accept() call -> accept connection request */
            communication_socket_fd = accept(master_socket_fd, NULL, NULL);
            if(communication_socket_fd == -1){
                perror("New connnection request fail\n");
                exit(EXIT_FAILURE);
            }
            /* Add data socket to monitor fd set */
            add_to_monitor_fd_set(communication_socket_fd);
        }
        /* If there's data on other client */
        else
        {
            temp_fd = -1;
            response.result = 0;
            /* Find which client send request in monitor fd set */
            for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
                if(FD_ISSET(monitor_fd_set[index], &read_fd)){
                    temp_fd = monitor_fd_set[index];
                    /* Clear buffer */
                    memset(buffer, 0, BUFFER_SIZE);
                    printf("Reading for data from client \n");
                    /* Waiting for data from client by call a blocking system call read() */
                    ret_val = recvfrom(temp_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr , &addrlen);
                    printf("Server received %d bytes from client with addr %s and port %d\n", \
                                    ret_val, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    /* Receive 0 bytes from client*/
                    if (0 == ret_val){
                        printf("Receive 0 bytes from client\n");
                        close(temp_fd);
                        remove_from_monitor_fd_set(temp_fd);
                        printf("Server close client connection with addr %s and port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                        break;
                    }
                    /* Clone data to struct data_to_server_t */
                    data_to_server_t *temp_data = (data_to_server_t *) buffer;
                    if((0 == temp_data->num1) && (0 == temp_data->num2)){
                        close(temp_fd);
                        remove_from_monitor_fd_set(temp_fd);
                        printf("Server close client connection with addr %s and port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                        break;
                    }
                    response_back_to_client response;
                    response.result = temp_data->num1 + temp_data->num2;
                    ret_val = sendto(temp_fd, (char *)&response, sizeof(response_back_to_client), 0, (struct sockaddr *)&client_addr , sizeof(struct sockaddr));
                    printf("Server send result back to client with addr %s and port %d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                }
            }
        }
    } /* Go back to select() and block */
    close(master_socket_fd);
    remove_from_monitor_fd_set(master_socket_fd);
    printf("Server exit\n");
    exit(EXIT_SUCCESS);
}