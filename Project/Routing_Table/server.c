#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include "common.h"

#define SOCKET_NAME "socketdemo/unixsocket_test"

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
    routing_table_list_struct_t head;

    /* Init linked list */
    head = init_head(head);
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
    /* Input from console */
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
            while(1)
            {
                ret_val = false;
                msg_t *msg_inf = (msg_t *)malloc(sizeof(msg_t));
                memset(buffer, 0, BUFFER_SIZE);
                /* Choose these following options to send service to client*/
                printf("Choose one option to send service to client:\n");
                printf("C(create) <DesIP> <Mask(0-32)> <GatewayIP> <OIF>\n");
                printf("U(update) <DesIP> <Mask(0-32)> <New GatewayIP> <New OIF>\n");
                printf("D(delete) <DesIP> <Mask(0-32)>\n");
                printf("S(show)\n");
                printf("F(flush)\n");
                printf("\n");
                /* Ex: [C 122.1.1.1 32 10.1.1.1 Ethernet1] */
                printf("Enter your service sever want to send to client\n");
                fgets(buffer, BUFFER_SIZE, stdin);
                /* Check format of input string */
                if(check_format_input_string(buffer, msg_inf) == true){
                    routing_table_struct_t local_data = (routing_table_struct_t )malloc(sizeof(data_info_t));
                    local_data->msg = msg_inf->data;
                    switch (msg_inf->opcode)
                    {
                    case (CREATE):
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            add_last_entry_list(head, local_data);
                            ret_val = true;
                        }
                        break;
                    case (UPDATE):
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            update_entry_table(head, local_data);
                        }
                        break;
                    case (DELETE):
                        if(check_condition(head, msg_inf->data, msg_inf->opcode) == true){
                            remove_node_list(head, local_data);
                            ret_val = true;
                        }
                        break;
                    case (SHOW):
                        show_routing_table_info(head);
                        break;
                    case (FLUSH):
                        flush_routing_table_info(head);
                        break;
                    default:
                        break;
                    }
                    if(ret_val == true)
                    {
                        ret_val = write(data_socket, buffer, BUFFER_SIZE);
                    }
                    free(msg_inf);
                    free(local_data);
                }
            }

        }
        else if (FD_ISSET(0, &read_fd)){ /* input from console*/
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
                    msg_t *temp_data = (msg_t *)buffer;
                    if(CREATE == temp_data->opcode){
                        memset(buffer, 0, BUFFER_SIZE);
                        ret_val = write(temp_fd, buffer, BUFFER_SIZE);
                        if(ret_val == -1){
                            perror("Read data fail\n");
                            exit(EXIT_FAILURE);
                        }
                        close(temp_fd);
                        /* Remove current client from monitor fd set */
                        remove_from_monitor_fd_set(temp_fd);
                        /* go to select() and block */
                        continue;
                    }
                }
            }
        }
    } /* Go back to select() and block */
    free(head);
    close(connection_socket);
    remove_from_monitor_fd_set(connection_socket);
    printf("Server exit\n");
    /* Release resources -> unlink socket */
    unlink(SOCKET_NAME);
    exit(EXIT_SUCCESS);
}
