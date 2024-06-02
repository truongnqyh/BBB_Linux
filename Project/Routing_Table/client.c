#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include "common.h"

#define NO_DATA 'NULL'
#define SOCKET_NAME "socketdemo/unixsocket_test"

int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int data_socket;
    int ret_val;
    int index;
    int input;
    int count;
    char buffer[BUFFER_SIZE];
    char buffer_send[BUFFER_SIZE];
    routing_table_list_struct_t head = NULL;
    int ret;

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
    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        /* Waiting operation request from server */
        printf("Waiting operation request from server:\n");
        ret_val = read(data_socket, buffer, BUFFER_SIZE);
        printf("Received request: %s\n", buffer);
        ret_val = false;
        msg_t *msg_inf = (msg_t *)calloc(1, sizeof(msg_t));
        /* Check format of input string */
        if(check_format_input_string(buffer, msg_inf) == true){
            routing_table_struct_t local_data = (routing_table_struct_t )malloc(sizeof(data_info_t));
            local_data->msg = msg_inf->data;
            switch (msg_inf->opcode)
            {
            case (CREATE):
                if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                    head = add_last_entry_list(head, local_data);
                    ret_val = true;
                }
                break;
            case (UPDATE):
                if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                    head = update_entry_table(head, local_data);
                    ret_val = true;
                }
                break;
            case (DELETE):
                if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                    head = remove_node_list(head, local_data);
                    ret_val = true;
                }
                break;
            case (SHOW):
                show_routing_table_info(head);
                ret_val = true;
                break;
            case (FLUSH):
                head = flush_routing_table_info(head);
                ret_val = true;
                break;
            default:
                break;
            }
            free(msg_inf);
        }
    }

    /* Close socket. */
    close(data_socket);
    exit(EXIT_SUCCESS);

}