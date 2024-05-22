#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SOCKET_NAME "socketdemo/unixsocket_test"
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32
int client_response = 0;

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int connection_socket;
    int data_socket;
    int ret_val;
    int temp_data;
    int index;
    char buffer[BUFFER_SIZE];

    /* Guarantee there's no socket from the last run */
    unlink(SOCKET_NAME);
    /* Create master socket */
    /** -->> SOCK_DGRAM for datagram based communication ***/
    connection_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if(connection_socket == -1){
        perror("Create master socket fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Create master socket successfully\n");
    /* Initialize server address with value 0 */
    memset(&server_address, 0, sizeof(server_address));
    /* Add properties for sockaddr_un */
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_NAME, sizeof(server_address.sun_path) - 1);
    /* Bind master socket to server address */
    ret_val = bind(connection_socket, (struct sockaddr *) &server_address, sizeof(server_address));
    if(ret_val == -1){
        perror("bind fail\n");
        exit(EXIT_FAILURE);
    }
    printf("bind successfully\n");
    /* Listen() call -> prepare for accepting connections */
    /* The backlog size is set to 20 then while one request is being 
    processed, the next request can be waiting */
    ret_val = listen(connection_socket, 20);
    if(ret_val == -1){
        perror("bind fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Listen successfully\n");

    /* Server work 24*7 then shoule be put in an infinite loop */
    while(1){
        /* Accept() call -> accept connection request */
        printf("New connection request, blocking at accept() \n");
        data_socket = accept(connection_socket, NULL, NULL);
        if(data_socket == -1){
            perror("New connnection request fail\n");
            exit(EXIT_FAILURE);
        }
        while(1){
            /* Clear buffer */
            memset(buffer, 0, BUFFER_SIZE);
            printf("Waiting for data from client \n");
            /* Waiting for data from client by call a blocking system call read() */
            ret_val = read(data_socket, buffer, BUFFER_SIZE);
            if(ret_val == -1){
                perror("Read data fail\n");
                exit(EXIT_FAILURE);
            }
            memcpy(&temp_data, buffer, sizeof(int));
            if(temp_data == 0){
                memset(buffer, 0, BUFFER_SIZE);
                sprintf(buffer, "Result = %d", client_response);
                printf("Send final result back to client %s\n");
                ret_val = write(data_socket, buffer, BUFFER_SIZE);
                if(ret_val == -1){
                    perror("Read data fail\n");
                    exit(EXIT_FAILURE);
                }
                close(data_socket);
                client_response = 0;
                /* go to accept() and block */
                break;
            }
            /* Sum temp_data to client_response and wait for next request*/
            client_response += temp_data;
        }
    } /* Go back to accept() and block */

    close(connection_socket);
    printf("Server exit\n");
    /* Release resources -> unlink socket */
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}