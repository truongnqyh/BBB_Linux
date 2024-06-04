#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdbool.h>
#include "common.h"
#include <signal.h>

#define NO_DATA 'NULL'
#define SOCKET_NAME "socketdemo/unixsocket_test"

routing_table_list_struct_t head = NULL;
mac_table_list_struct_t head_mac = NULL;
int monitor_fd_set[MAX_CLIENTS_SUPPORT];
int client_response[MAX_CLIENTS_SUPPORT] = {0};
int data_socket;
pid_t pid;

void signal_handler(int SIGNUM)
{
    if(SIGNUM == SIGINT){
        write(data_socket, &pid, sizeof(pid_t));
        close(data_socket);
        printf("Client exit by sigint\n");
        exit(EXIT_SUCCESS);
    }

    if(SIGNUM == SIGUSR1){
        head = flush_routing_table_info(head);
        head_mac = flush_mac_info(head_mac);
    }
}

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int ret_val;
    int index;
    int input;
    int count;
    int is_RT_MAC;
    char buffer[BUFFER_SIZE];
    char buffer_send[BUFFER_SIZE];
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
    /* Send process's ID to client */
    pid = getpid();
    write(data_socket, &pid, sizeof(pid_t)); // send server client's process id
    signal(SIGINT, signal_handler);  //register signal handler
    signal(SIGUSR1, signal_handler);    
    printf("Connect data socket successfully\n");
    while(1){
        memset(buffer, 0, BUFFER_SIZE);
        /* Waiting operation request from server */
        printf("Waiting operation request from server:\n");
        ret_val = read(data_socket, buffer, BUFFER_SIZE);
        printf("Received request: %s\n", buffer);
        msg_t *msg_inf = (msg_t *)calloc(1, sizeof(msg_t));
        mac_table_struct_t mac_info = (mac_table_struct_t)calloc(MAC_ADDR_LEN, sizeof(char));
        /* Check format of input string */
        if(check_format_input_string(buffer ,msg_inf, mac_info, &is_RT_MAC) == true){
            routing_table_struct_t local_data = (routing_table_struct_t )malloc(sizeof(data_info_t));
            local_data->msg = msg_inf->data;
            switch (msg_inf->opcode)
            {
            case (CREATE):
                if(is_RT_MAC == IS_MAC){
                    if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                        head_mac = add_last_entry_list_mac(head_mac, mac_info);
                    }
                }
                else{
                    if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                        head = add_last_entry_list(head, local_data);
                    }
                }
                break;
            case (UPDATE):
                if(is_RT_MAC == IS_MAC){
                    if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                        head_mac = update_entry_table_mac(head_mac, mac_info);
                    }
                }
                else{
                    if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                        head = update_entry_table(head, local_data);
                    }
                }
                break;
            case (DELETE):
                if(is_RT_MAC == IS_MAC){
                    if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                        head_mac = remove_node_list_mac(head_mac, mac_info);
                    }
                }
                else{
                    if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                        head = remove_node_list(head, local_data);
                    }
                }
                break;
            case (SHOW):
                show_routing_table_info(head);
                printf("******************************************************\n");
                show_mac_info(head_mac);
                break;
            case (FLUSH):
                head = flush_routing_table_info(head);
                head_mac = flush_mac_info(head_mac);
                break;
            case (QUIT):
                head = flush_routing_table_info(head);
                head_mac = flush_mac_info(head_mac);
                free(msg_inf);
                goto DISCONNECT;
                break;

            default:
                break;
            }
            free(msg_inf);
        }
    }
DISCONNECT:
    /* Close socket. */
    close(data_socket);
    printf("Client exit\n");
    exit(EXIT_SUCCESS);

}