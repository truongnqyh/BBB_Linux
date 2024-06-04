/* RTM, ARP table */
/* Mac addr example: XX:XX:XX:XX:XX:XX */
/* Ip addr example: X.X.X.X */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include "common.h"
#include <signal.h>

#define SOCKET_NAME "socketdemo/unixsocket_test"

routing_table_list_struct_t head = NULL;
mac_table_list_struct_t head_mac = NULL;
int connection_socket;

void signal_handler(int SIGNUM){
    if(SIGNUM == SIGINT){
        char buffer[] = "Q";
        int index = 0;

        for(index; index < MAX_CLIENTS_SUPPORT; index++){
            if((monitor_fd_set[index] != -1) \
            && (monitor_fd_set[index] != 0) \
            && (monitor_fd_set[index] != connection_socket)){
               write(monitor_fd_set[index], buffer, BUFFER_SIZE);
            }
        }
        close(connection_socket);
        remove_from_monitor_fd_set(connection_socket);
        printf("Server exit\n");
        exit(EXIT_SUCCESS);
    }
}

int main (int argc, char *argv[]){
    struct sockaddr_un server_address;
    int data_socket;
    int ret_val;
    fd_set read_fd;
    int index;
    int temp_fd;
    int temp_data;
    pid_t temp_pid;
    char buffer[BUFFER_SIZE];
    int is_RT_MAC;

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
    /* Init client pid set with value -1 */
    init_client_pid_set();
    /* Init monitor fd set with value -1 */
    init_monitor_fd_set();
    /* Input from console */
    add_to_monitor_fd_set(0);
    /* Add master socket to monitor fd set */
    add_to_monitor_fd_set(connection_socket);
    signal(SIGINT, signal_handler);
    /* Server work 24*7 then shoule be put in an infinite loop */
    while(1){
        printf("--------------------------------------------------------\n");
        /* Choose these following options to send service to client*/
        /* Ex: [C 122.1.1.1 32 10.1.1.1 Ethernet1] */
        printf("Choose one option to send request to server:\n");
        printf("C(create) <DesIP> <Mask(0-32)> <GatewayIP> <OIF>\n");
        printf("U(update) <DesIP> <Mask(0-32)> <New GatewayIP> <New OIF>\n");
        printf("D(delete) <DesIP> <Mask(0-32)>\n");
        printf("S(show)\n");
        printf("F(flush)\n");
        printf("\n");
        /* Clone to fd_set type to be an argument for select() */
        clone_to_fd_set(&read_fd);
        /* Waiting select() to be unblocked */
        printf("Waiting on select() sys call\n");
        printf("--------------------------------------------------------\n");
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
            /* Send RTM table to connected client */
            printf("Sending RTM table to connected clients\n");
            send_rtable_to_newly_client(data_socket, head);
            printf("Sending ARP table to connected clients\n");
            send_mac_table_to_newly_client(data_socket, head_mac);
        }
        else if (FD_ISSET(0, &read_fd)){ /* input from console*/
            printf("User typed\n");
            /* Clear buffer */
            memset(buffer, 0, BUFFER_SIZE);
            /* Receive data from current client */
            ret_val = read(0, buffer, BUFFER_SIZE);
            printf("Input buffer: %s\n");
            char *copy_buffer = (char *)malloc(sizeof(char) * BUFFER_SIZE);
            strcpy(copy_buffer, buffer);
            ret_val = false;
            msg_t *msg_inf = (msg_t *)calloc(1, sizeof(msg_t)); 
            mac_table_struct_t mac_info = (mac_table_struct_t)calloc(MAC_ADDR_LEN, sizeof(char));
            /* Check format of input string */
            if(check_format_input_string(copy_buffer, msg_inf, mac_info, &is_RT_MAC) == true){
                routing_table_struct_t local_data = (routing_table_struct_t )malloc(sizeof(data_info_t));
                local_data->msg = msg_inf->data;
                switch (msg_inf->opcode)
                {
                case (CREATE):
                    if(is_RT_MAC == IS_MAC){
                        if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                            char ip_value[IP_ADDR_LEN];
                            printf("Enter CREATE ip value for corresponding MAC addr: %s\n", mac_info->mac);
                            ret_val = read(0, ip_value, IP_ADDR_LEN);
                            if(check_desIP_valid(ip_value) == true){
                                if(store_IP_to_Shm(mac_info->mac, ip_value) == true){
                                    head_mac = add_last_entry_list_mac(head_mac, mac_info);
                                    ret_val = true;
                                }
                            }
                        }
                    }
                    else{
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            head = add_last_entry_list(head, local_data);
                            ret_val = true;
                        }
                    }
                    break;
                case (UPDATE):
                    if(is_RT_MAC == IS_MAC){
                        if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                            char ip_value[IP_ADDR_LEN];
                            printf("Enter UPDATE ip value for corresponding MAC addr: %s\n", mac_info->mac);
                            ret_val = read(0, ip_value, IP_ADDR_LEN);
                            if(check_desIP_valid(ip_value) == true){
                                if(update_IP_in_Shm(mac_info->mac, ip_value) == true){
                                    head_mac = update_entry_table_mac(head_mac, mac_info);
                                    ret_val = true;
                                }
                            }
                        }
                    }
                    else{
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            head = update_entry_table(head, local_data);
                            ret_val = true;
                        }
                    }
                    break;
                case (DELETE):
                    if(is_RT_MAC == IS_MAC){
                        if(check_condition_mac(head_mac, mac_info->mac, msg_inf->opcode) == true){
                            head_mac = remove_node_list_mac(head_mac, mac_info);
                            ret_val = true;
                        }
                    }
                    else{
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            head = remove_node_list(head, local_data);
                            ret_val = true;
                        }
                    }
                    break;
                case (SHOW):
                    show_routing_table_info(head);
                    printf("******************************************************\n");
                    show_mac_info(head_mac);
                    show_pid_table();
                    ret_val = true;
                    break;
                case (FLUSH):
                    head = flush_routing_table_info(head);
                    head_mac = flush_mac_info(head_mac);
                    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
                        if(client_pid_set[index] != -1){
                            kill(client_pid_set[index], SIGUSR1);
                        }
                    }
                    break;
                default:
                    break;
                }
                if (ret_val == true){
                    /* Send to remaining clients package from this client */
                    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
                        if((monitor_fd_set[index] != -1) \
                        && (monitor_fd_set[index] != 0) \
                        && (monitor_fd_set[index] != connection_socket)){
                            ret_val = write(monitor_fd_set[index], buffer, BUFFER_SIZE);
                        }
                    }
                }
                free(msg_inf);
                free(copy_buffer);
            }
        }
        /* If there's data on other client */
        else{
            temp_fd = -1;
            temp_pid = 0;
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
                    memcpy(&temp_pid, buffer, sizeof(int));
                    if(check_available_pid(temp_pid) == true){
                        printf("Client connect with PID: %d\n", temp_pid);
                        add_to_pid_set(temp_pid);
                    }
                    else{
                        printf("Client disconnect with PID: %d\n", temp_pid);
                        remove_from_client_pid_set(temp_pid);
                        remove_from_monitor_fd_set(temp_fd);
                    }
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
