#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>

#define SOCKET_NAME "tmp/unixsocket_test"
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32

int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};


static void init_monitor_fd_set(){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        monitor_fd_set[index] == -1; 
    }
}

static void add_to_monitor_fd_set(int connection_socket){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        if(monitor_fd_set[index] == -1){
            monitor_fd_set[index] = connection_socket;
            break;
        }
    }
}

static void clear_monitor_fd_set(){

}

static void refresh_fd_set(fd_set *p_fd_set){
    int index = 0;

    FD_ZERO(p_fd_set);
    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
        if(monitor_fd_set[index] != -1){
            FD_SET(monitor_fd_set[index], p_fd_set);
        }
    }

}

static int max_fd(){

}

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int connection_socket;
    int data_socket;
    int ret_val;

    /* Guarantee there's no socket from the last run */
    unlink(SOCKET_NAME);
    /* Create master socket */
    /** -->> SOCK_DGRAM for datagram based communication ***/
    connection_socket = socket(AF_UNIX, SOCK_STREAM, NULL);
    if(connection_socket = -1){
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
    /* Init monitor fd set with value -1 */
    init_monitor_fd_set();
    /* Add master socket to monitor fd set */
    add_to_monitor_fd_set(connection_socket);

    while(1){

    }
}