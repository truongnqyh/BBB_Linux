#include "common.h"

int monitor_fd_set[MAX_CLIENTS_SUPPORT];
msg_t routing_table_info[MAX_DESTINATION];


static routing_table_list_struct_t create_new_entrydir_node(const routing_table_struct_t inputEntryInfo);
static routing_table_struct_t create_new_info(data_info_t msg_info);
static routing_table_list_struct_t add_entry_dir(routing_table_list_struct_t head, data_info_t msg_info);

void clone_to_fd_set(fd_set *fd_set){
    int index = 0;

    FD_ZERO(fd_set);
    for(index = 0; index < MAX_CLIENTS_SUPPORT; index++){
        if(monitor_fd_set[index] != -1){
            FD_SET(monitor_fd_set[index], fd_set);
        }
    }
}

void init_monitor_fd_set(){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        monitor_fd_set[index] = -1; 
    }
}

void add_to_monitor_fd_set(int fd){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        if(monitor_fd_set[index] == -1){
            monitor_fd_set[index] = fd;
            break;
        }
    }
}

void remove_from_monitor_fd_set(int fd){
    int index = 0;

    for(index = 0; index < MAX_CLIENTS_SUPPORT; index ++){
        if(monitor_fd_set[index] == fd){
            monitor_fd_set[index] = -1;
            break;
        }
    }
}

int max_fd(){
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

static routing_table_list_struct_t add_entry_dir(routing_table_list_struct_t head, data_info_t msg_info)
{
    routing_table_struct_t entryInfo = NULL;

    entryInfo = create_new_info(msg_info);
    head = add_last_entry_list(head, entryInfo);

    return head;
}

routing_table_list_struct_t init_head(routing_table_list_struct_t head)
{
    head = (routing_table_list_struct_t)malloc(sizeof(struct routing_table_list_struct));
    head = NULL;
    return head;
}

static routing_table_list_struct_t create_new_entrydir_node(const routing_table_struct_t inputEntryInfo)
{
    routing_table_list_struct_t temp = NULL; // declare a node

    temp = (routing_table_list_struct_t)malloc(sizeof(struct routing_table_list_struct));
    temp->next = NULL;
    temp->msg_info = inputEntryInfo;

    return temp;
}

routing_table_list_struct_t add_last_entry_list(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo)
{
    routing_table_list_struct_t temp;
    routing_table_list_struct_t ptr;

    temp = create_new_entrydir_node(inputEntryInfo);
    ptr = head;
    if (NULL == ptr)
    {
        head = temp;
    }
    else
    {
        while (ptr->next != NULL)
        {
            ptr = ptr->next; /* last node->next = NULL*/
        }
        ptr->next = temp; /* last node -> temp*/
    }

    return head;
}

routing_table_list_struct_t update_entry_table(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo)
{
    routing_table_list_struct_t ptr;

    ptr = head; 
    while (ptr != NULL)
    {
        if((strcmp(ptr->msg_info->msg.destination, inputEntryInfo->msg.destination) == 0) \
                    && (strcmp(ptr->msg_info->msg.mask, inputEntryInfo->msg.mask) == 0)){
            /* Update routing table */
            ptr->msg_info->msg = inputEntryInfo->msg;
        }
        ptr = ptr->next; /* last node->next = NULL*/
    }
    
    return head;
}

routing_table_list_struct_t remove_node_list(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo)
{
    routing_table_list_struct_t ptr;
    routing_table_list_struct_t last_ptr;
    int count = 0;

    ptr = head;
    while (ptr != NULL){
        if((strcmp(ptr->msg_info->msg.destination, inputEntryInfo->msg.destination) == 0) \
                    && (strcmp(ptr->msg_info->msg.mask, inputEntryInfo->msg.mask) == 0)){
            if(count == 0){
                if(ptr->next == NULL){
                    head = NULL;
                }
                else
                {
                    head = ptr->next;
                }
            }
            else{
                if(ptr->next == NULL)
                {
                    last_ptr->next = NULL;
                }
                else
                {
                    last_ptr->next = ptr->next;
                }
            }
            free(ptr->msg_info);
            free(ptr);
            break;
        }
        count ++;
        last_ptr = ptr; /* last node */
        ptr = ptr->next; /* last node -> next */
    }

    return head;
}

static routing_table_struct_t create_new_info(data_info_t msg_info)
{
    routing_table_struct_t newEntryInfo = NULL;

    newEntryInfo = (routing_table_struct_t)malloc(sizeof(struct routing_table_struct));
    if(newEntryInfo == NULL){
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    newEntryInfo->msg = msg_info;
    printf("%s\n", msg_info.destination);
    printf("%s\n", msg_info.mask);
    printf("%s\n", msg_info.gateway_ip);
    printf("%s\n", msg_info.OIF);
     printf("%s\n", newEntryInfo->msg.destination);
    printf("%s\n", newEntryInfo->msg.mask);
    printf("%s\n", newEntryInfo->msg.gateway_ip);
    printf("%s\n", newEntryInfo->msg.OIF);

    return newEntryInfo;
}

bool check_condition(routing_table_list_struct_t head, data_info_t msg_info, FUNCTION opcode)
{
    routing_table_list_struct_t ptr;
    bool ret_val = true;
    int count = 0;

    switch (opcode){
        case (CREATE):
            ptr = head;
            if(NULL == ptr)
            {
                break;
            }
            while (ptr->next != NULL)
            {
                if((strcmp(ptr->msg_info->msg.destination, msg_info.destination) == 0) \
                && (strcmp(ptr->msg_info->msg.mask, msg_info.mask) == 0)){
                    printf("This node already have routing table\n");
                    ret_val = false;
                    break;
                }
                ptr = ptr->next;
            }
            break;
        case (UPDATE):
            ptr = head;
            if(NULL == ptr){
                printf("The table is empty, there's nothing to update\n");
                ret_val = false;
            }
            else{
                while (ptr->next != NULL)
                {
                    if((strcmp(ptr->msg_info->msg.destination, msg_info.destination) == 0) \
                    && (strcmp(ptr->msg_info->msg.mask, msg_info.mask) == 0)){
                        count++;
                        break;
                    }
                    ptr = ptr->next;
                }
                if(count == 0){
                    printf("There's no node to update\n");
                    ret_val = false;
                }
            }
            break;
        case (DELETE):
            ptr = head;
            if(NULL == head){
                printf("The table is empty, there's nothing to remove\n");
                ret_val = false;
            }
            else
            {
                while (ptr != NULL){
                    if((strcmp(ptr->msg_info->msg.destination, msg_info.destination) == 0) \
                    && (strcmp(ptr->msg_info->msg.mask, msg_info.mask) == 0)){
                        ret_val = true;
                        break;
                    }
                    ptr = ptr->next; /* last node->next */
                }
                if(false == ret_val){
                    printf("Message information not found in routing table\n");
                }
            }
            break;
        default:
            break;
    }

    return ret_val;
}

bool check_remove_condition(routing_table_list_struct_t head, data_info_t msg_info)
{
    routing_table_list_struct_t ptr;
    bool ret_val = false;


    return ret_val;
}

void show_routing_table_info(routing_table_list_struct_t head)
{
    routing_table_list_struct_t ptr;
    char dest_mask[24];
    ptr = head;
    if(NULL == ptr){
        printf("------> The table is empty, there's nothing to show\n");
    }
    else{ 
        printf("%-24s%-24s%-24s\n","Destination/Mask      |","GatewayIP             |","OIF                   |");
        printf("%-24s%-24s%-24s\n","----------------------|","----------------------|","----------------------|");
        while (ptr != NULL)
        {
            strcpy(dest_mask, ptr->msg_info->msg.destination);
            strcat(dest_mask, "/");
            strcat(dest_mask, ptr->msg_info->msg.mask);
            printf("%-22s| %-22s| %-22s|\n",dest_mask, ptr->msg_info->msg.gateway_ip, ptr->msg_info->msg.OIF);
            ptr = ptr->next;
        }
    }
}

routing_table_list_struct_t flush_routing_table_info(routing_table_list_struct_t head)
{
    routing_table_list_struct_t ptr;
    routing_table_list_struct_t last_ptr;

    ptr = head;
    while (ptr != NULL)
    {  
        last_ptr = ptr;
        ptr = ptr -> next;
        free(last_ptr->msg_info);
        free(last_ptr);
    }
    head = NULL;

    return head;
}

bool check_desIP_valid(char dest[DEST_SIZE])
{
    bool ret_val = true;
    int count = 0;
    char dest_test[DEST_SIZE];
    char *token1;

    memcpy(dest_test, dest, DEST_SIZE);
    if(strlen(dest_test) > DEST_SIZE){
        printf("Destination IP address is too long\n");
        ret_val = false;
    }
    else{
        /* Ex 127.0.0.1 -> 3 times "." */
        token1 = strtok(dest_test, ".");
        while(token1 != NULL)
        {
            count ++;
            token1 = strtok(NULL, ".");
        }
        if(4 != count)
        {
            printf("Destination IP address format is wrong\n");
            ret_val = false;
        }
    }

    return ret_val;
}

bool check_mask_valid(char* mask, int size) {
    char mask_check[size];
    int number;
    bool ret_val = true;

    memcpy(mask_check, mask, size);
    number = atoi(mask_check);
    if((number < 0 ) || (number > 32))
    {
        ret_val = false;
    }

    return ret_val;
}

bool check_gateway_valid(char* gateway)
{
    bool ret_val = true;
    int count = 0;
    char gateway_test[GATEW_SIZE];
    char *token;

    memcpy(gateway_test, gateway, GATEW_SIZE);
    if(strlen(gateway_test) > GATEW_SIZE){
        printf("Gateway IP address is too long\n");
        ret_val = false;
    }
    else{
        /* Ex 10.0.0.1 -> 3 times "." */
        token = strtok(gateway_test, ".");
        while(token != NULL)
        {
            count ++;
            token = strtok(NULL, ".");
        }
        if(4 != count)
        {
            printf("Gateway IP address format is wrong\n");
            ret_val = false;
        }
    }

    return ret_val;
}

bool check_OIF_valid(char* oif)
{
    bool ret_val = true;
    int length;

    length = strlen(oif);
    if(length > OIF_SIZE)
    {
        ret_val = false;
    }

    return ret_val;
}

bool check_format_input_string(char input_string[BUFFER_SIZE], msg_t *msg_info)
{
    bool ret_val = true;
    char *token;
    char *token_check;
    size_t lenght;
    int index = 0;
    char* result;
    int mask_size;

    token = strtok(input_string, " ");
    if(token != NULL){
        switch (*token){
            case 'C':
                msg_info->opcode = CREATE;
                for(index = 0; index < 4; index ++)
                {
                    token = strtok(NULL, " ");
                    if(token == NULL){
                        ret_val = false;
                    }
                    else{
                        switch ((index))
                        {
                        case (0):
                            memcpy(msg_info->data.destination, token, strlen(token) + 1);
                            break;
                        case (1):
                            mask_size = strlen(token);
                            if(mask_size > 2)
                            {
                                printf("Mask is too long\n");
                                ret_val = false;
                                break;
                            }
                            memcpy(msg_info->data.mask, token, strlen(token) + 1);
                            break;
                        case (2):
                            memcpy(msg_info->data.gateway_ip, token, strlen(token) + 1);
                            break;
                        case (3):
                            result = strchr(token, '\n');
                            if(result == NULL)
                            {
                                printf("Input string format is wrong\n");
                                ret_val = false;
                            }
                            else
                            {
                                /* minus -1 ~ '\n' */
                                memcpy(msg_info->data.OIF, token, strlen(token) - 1);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }

                if(ret_val == true)
                {
                    if((check_desIP_valid(msg_info->data.destination) == false) \
                    || (check_mask_valid(msg_info->data.mask, mask_size) == false) \
                    || (check_gateway_valid(msg_info->data.gateway_ip) == false) \
                    || (check_OIF_valid(msg_info->data.OIF) == false))
                    {
                        ret_val = false;
                    }
                    
                }
                break;
            case 'U':
                msg_info->opcode = UPDATE;
                for(index = 0; index < 4; index ++)
                {
                    token = strtok(NULL, " ");
                    if(token == NULL){
                        ret_val = false;
                    }
                    else{
                        switch ((index))
                        {
                        case (0):
                            memcpy(msg_info->data.destination, token, strlen(token) + 1);
                            break;
                        case (1):
                            mask_size = strlen(token);
                            if(mask_size > 2)
                            {
                                printf("Mask is too long\n");
                                ret_val = false;
                                break;
                            }
                            memcpy(msg_info->data.mask, token, strlen(token) + 1);
                            break;
                        case (2):
                            memcpy(msg_info->data.gateway_ip, token, strlen(token) + 1);
                            break;
                        case (3):
                            result = strchr(token, '\n');
                            if(result == NULL)
                            {
                                ret_val = false;
                            }
                            else
                            {
                                /* minus -1 ~ '\n' */
                                memcpy(msg_info->data.OIF, token, strlen(token) - 1);
                            }
                            break;
                        default:
                            break;
                        }
                    }
                }

                if(ret_val == true)
                {
                    if((check_desIP_valid(msg_info->data.destination) == false) \
                    || (check_mask_valid(msg_info->data.mask, mask_size) == false) \
                    || (check_gateway_valid(msg_info->data.gateway_ip) == false) \
                    || (check_OIF_valid(msg_info->data.OIF) == false))
                    {
                        ret_val = false;
                    }
                    
                }
                break;
            case 'D':
                msg_info->opcode = DELETE;
                for(index = 0; index < 2; index ++)
                {
                    token = strtok(NULL, " ");
                    if(token == NULL){
                        ret_val = false;
                    }
                    else{
                        switch ((index))
                        {
                        case (0):
                            memcpy(msg_info->data.destination, token, strlen(token) + 1);
                            break;
                        case (1):
                            /* should be 1\n or 31\n -> length should be 3 or 2 */
                            mask_size = strlen(token) - 1;
                            if((mask_size != 1) && (mask_size != 2))
                            {
                                printf("Mask format of string is wrong\n");
                                ret_val = false;
                                break;
                            }
                            memcpy(msg_info->data.mask, token, mask_size);
                            break;
                        default:
                            break;
                        }
                    }
                }
                token = strtok(NULL, " ");
                if(token != NULL)
                {
                    printf("Input string format is wrong\n");
                    ret_val = false;
                }
                if(ret_val == true)
                {
                    if((check_desIP_valid(msg_info->data.destination) == false) \
                    || (check_mask_valid(msg_info->data.mask, mask_size) == false))
                    {
                        ret_val = false;
                    }
                    
                }
                break;
            case 'S':
                msg_info->opcode = SHOW;
                token = strtok(NULL, " ");
                if((token == NULL)){

                }
                else{
                    printf("invalid S(show) option\n");
                }
                break;
            case 'F':
                msg_info->opcode = FLUSH;
                token = strtok(NULL, " ");
                if(token == NULL){

                }
                else{
                    printf("invalid F(flush) option\n");
                }
                break;
            default:
                printf("Invalid opcode\n");
                ret_val = false;
        }
    }

    return ret_val;
}

void send_rtable_to_newly_client(int fd, routing_table_list_struct_t head){
    routing_table_list_struct_t ptr;
    char buffer[BUFFER_SIZE];

    ptr = head;
    while(ptr != NULL)
    {
        sprintf(buffer, "C %s %s %s %s\n", \
        ptr->msg_info->msg.destination, \
        ptr->msg_info->msg.mask, \
        ptr->msg_info->msg.gateway_ip, \
        ptr->msg_info->msg.OIF);
        printf("%s\n", buffer);
        write(fd, buffer, BUFFER_SIZE);
        ptr = ptr->next;
    }
}
