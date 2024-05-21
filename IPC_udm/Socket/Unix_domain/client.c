#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_NAME "tmp/unixsocket_test"
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32
#define CURRENT_CLIENT 0
int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int data_socket;
    int ret_val;
    int index;
    int input;
    char buffer[BUFFER_SIZE];

    /* Create data socket */
    data_socket = socket(AF_UNIX, SOCK_STREAM, 0);  
    if(data_socket == -1){
        perror("Create data socket fail\n");
        exit(EXIT_FAILURE);
    }
    memset(&server_address, 0, sizeof(struct sockaddr_un));

    /* Connect socket to socket address */
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_NAME, sizeof(server_address.sun_path) - 1);
    ret_val = connect(data_socket, (struct sockaddr*)&server_address, sizeof(server_address));
    if (ret_val == -1) {
        perror("connect fail\n");
        exit(EXIT_FAILURE);
    }
    printf("Connect data socket successfully\n");
    /* Send data to server */
    do{
        printf("Enter the data to send to server\n");
        scanf("%d", &input);
        ret_val = write(data_socket, &input, sizeof(int));
        if (ret_val == -1) {
            perror("Write fail\n");
            exit(EXIT_FAILURE);
        }
        printf("Number of bytes sent: %d, data: %d\n", ret_val, input);
    }while(input != 0);

    /* Wait for the response from server */
    memset(buffer, 0, BUFFER_SIZE);
    
    ret_val = read(data_socket, buffer, BUFFER_SIZE);
    if (ret_val == -1) {
        perror("Read fail\n");
        exit(EXIT_FAILURE);
    }

    printf("%s\n", buffer);

    /* Close socket. */

    close(data_socket);

    exit(EXIT_SUCCESS);

}