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
#define CURRENT_CLIENT 0
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
    struct sockaddr_un server_address;
    int connection_socket;
    int data_socket;
    int ret_val;
    fd_set read_fd;
    int index;
    int temp_fd;
    int temp_data;
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
    /* Init monitor fd set with value -1 */
    init_monitor_fd_set();
    add_to_monitor_fd_set(0);
    /* Add master socket to monitor fd set */
    add_to_monitor_fd_set(connection_socket);
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
        if(FD_ISSET(connection_socket, &read_fd)){
            printf("New connection request\n");
            /* Accept() call -> accept connection request */
            data_socket = accept(connection_socket, NULL, NULL);
            if(data_socket == -1){
                perror("New connnection request fail\n");
                exit(EXIT_FAILURE);
            }
            /* Add data socket to monitor fd set */
            add_to_monitor_fd_set(data_socket);
        }
        else if (FD_ISSET(0, &read_fd)){
            /* Clear buffer */
            memset(buffer, 0, BUFFER_SIZE);
            /* Receive data from current client */
            ret_val = read(0, buffer, BUFFER_SIZE);
            printf("Input read from console: %s\n", buffer);
        }
        /* If there's data on other client */
        else
        {
            temp_fd = -1;
            temp_data = 0;
            /* Find which client send request in monitor fd set */
            for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
                if(FD_ISSET(monitor_fd_set[index], &read_fd)){
                    temp_fd = monitor_fd_set[index];
                    /* Clear buffer */
                    memset(buffer, 0, BUFFER_SIZE);
                    printf("Waiting for data from client \n");
                    /* Waiting for data from client by call a blocking system call read() */
                    ret_val = read(temp_fd, buffer, BUFFER_SIZE);
                    if(ret_val == -1){
                        perror("Read data fail\n");
                        exit(EXIT_FAILURE);
                    }
                    memcpy(&temp_data, buffer, sizeof(int));
                    if(temp_data == 0){
                        memset(buffer, 0, BUFFER_SIZE);
                        sprintf(buffer, "Result = %d", client_response[index]);
                        printf("Send final result back to client %s\n");
                        ret_val = write(temp_fd, buffer, BUFFER_SIZE);
                        if(ret_val == -1){
                            perror("Read data fail\n");
                            exit(EXIT_FAILURE);
                        }
                        close(temp_fd);
                        client_response[index] = 0;
                        /* Remove current client from monitor fd set */
                        remove_from_monitor_fd_set(temp_fd);
                        /* go to select() and block */
                        continue;
                    }
                    /* Sum temp_data to client_response and wait for next request*/
                    client_response[index] += temp_data;
                }
            }
        }
    } /* Go back to select() and block */
    close(connection_socket);
    remove_from_monitor_fd_set(connection_socket);
    printf("Server exit\n");
    /* Release resources -> unlink socket */
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}