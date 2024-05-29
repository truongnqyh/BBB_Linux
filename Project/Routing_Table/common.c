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
    if (head == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    head->msg_info = 0; // Set data to 0 (or any other desired value)
    head->next = NULL; // Set next pointer to NULL initially

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
    if (NULL == head)
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
        if((ptr->msg_info->msg.destination == inputEntryInfo->msg.destination) \
        && (ptr->msg_info->msg.mask == inputEntryInfo->msg.mask)){
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
        if((ptr->msg_info->msg.destination == inputEntryInfo->msg.destination) \
        && (ptr->msg_info->msg.mask == inputEntryInfo->msg.mask)){
            printf("Routing table already have this node\n");
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
            while (ptr->next != NULL)
            {
                if((ptr->msg_info->msg.destination == msg_info.destination) \
                && (ptr->msg_info->msg.mask == msg_info.mask)){
                    printf("routing table already have this node\n");
                    ret_val = false;
                    break;
                }
            }
            break;
        case (UPDATE):
            ptr = head;
            if(NULL == head){
                printf("The table is empty, there's nothing to update\n");
                ret_val = false;
            }
            else{
                while (ptr->next != NULL)
                {
                    if((ptr->msg_info->msg.destination == msg_info.destination) \
                    && (ptr->msg_info->msg.mask == msg_info.mask)){
                        count++;
                        break;
                    }
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
                    if((ptr->msg_info->msg.destination == msg_info.destination) \
                    && (ptr->msg_info->msg.mask == msg_info.mask)){
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

    ptr = head;
    if(NULL == head){
        printf("The table is empty, there's nothing to Show\n");
    }
    else{ 
        while (ptr != NULL)
        {
            char dest_mask[24];
            strcpy(dest_mask, ptr->msg_info->msg.destination);
            strcat(dest_mask, "/");
            strcat(dest_mask, ptr->msg_info->msg.mask);
            printf("%-24s%-24s%-24s\n","   Destination/Mask   |","      GatewayIP       |","         OIF          |");
            printf("%-24s%-24s%-24s\n","----------------------|","----------------------|","----------------------|");
            printf("%-24s%-24s%-24s\n",dest_mask, ptr->msg_info->msg.gateway_ip, ptr->msg_info->msg.OIF);
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
    char *token;

    if(strlen(dest) > DEST_SIZE){
        printf("Destination IP address is too long\n");
        ret_val = false;
    }
    else{
        /* Ex 127.0.0.1 -> 3 times "." */
        token = strtok(dest, ".");
        while(token != NULL)
        {
            count ++;
            token = strtok(NULL, ".");
        }
        if(4 != count)
        {
            printf("Destination IP address format is wrong\n");
            ret_val = false;
        }
    }

    return ret_val;
}

bool check_mask_valid(char* mask)
{
    unsigned int number;
    bool ret_val = true;

    /* Mask has to be in the range of 0 to 32 */
    number = atoi(mask);
    if(number > 32)
    {
        printf("Mask is too big, should be <= 32\n");
        ret_val = false;
    }

    return ret_val;
}

bool check_gateway_valid(char* gateway)
{
    bool ret_val = true;
    int count = 0;
    char *token;

    if(strlen(gateway) > GATEW_SIZE){
        printf("Gateway IP address is too long\n");
        ret_val = false;
    }
    else{
        /* Ex 10.0.0.1 -> 3 times "." */
        token = strtok(gateway, ".");
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

bool check_format_input_string(char input_string[BUFFER_SIZE], msg_t *msg_info)
{
    bool ret_val = true;
    char *token;
    char *token_check;
    size_t lenght;

    token = strtok(input_string, " ");
    if(token != NULL){
        switch (*token){
            case 'C':
                msg_info->opcode = CREATE;
                token = strtok(NULL, " ");
                lenght = strlen(token) + 1;
                token_check = (char *)malloc(lenght*sizeof(char));
                memcpy(token_check, token, lenght);
                if((check_desIP_valid(token_check) == false) || (token == NULL)){
                    ret_val = false;
                }
                else{
                    free(token_check);
                    memcpy(msg_info->data.destination, token, sizeof(token));
                    token = strtok(NULL, " ");
                    lenght = strlen(token) + 1;
                    token_check = (char *)malloc(lenght*sizeof(char));
                    memcpy(token_check, token, lenght);
                    if((check_mask_valid(token_check) == false) || (token == NULL)){
                        ret_val = false;
                    }
                    else{
                        free(token_check);
                        memcpy(msg_info->data.mask, token, sizeof(token));
                        token = strtok(NULL, " ");
                        memset(token_check, NULL ,sizeof(token));
                        memcpy(token_check, token, sizeof(token));
                        if((check_gateway_valid(token_check) == false) || (token == NULL)){
                            ret_val = false;
                        }
                        else{
                            memcpy(msg_info->data.gateway_ip, token, sizeof(token));
                            token = strtok(NULL, " ");
                            if(token == NULL){
                                ret_val = false;
                            }
                            else{
                                memcpy(msg_info->data.OIF, token, sizeof(token));
                                token = strtok(NULL, " ");
                                if(token != NULL){
                                    printf("Too many arguments\n");
                                    ret_val = false;
                                }
                            }
                        }
                    }
                }
                break;
            case 'U':
                msg_info->opcode = UPDATE;
                token = strtok(NULL, " ");
                if((check_desIP_valid(token) == false) || (token == NULL)){
                    ret_val = false;
                }
                else{
                    strcpy(msg_info->data.destination, token);
                    token = strtok(NULL, " ");
                    if((check_mask_valid(token) == false) || (token == NULL)){
                        ret_val = false;
                    }
                    else{
                        strcpy(msg_info->data.mask, token);
                        token = strtok(NULL, " ");
                        if((check_gateway_valid(token) == false) || (token == NULL)){
                            ret_val = false;
                        }
                        else{
                            strcpy(msg_info->data.gateway_ip, token);
                            token = strtok(NULL, " ");
                            if(token == NULL){
                                ret_val = false;
                            }
                            else{
                                strcpy(msg_info->data.OIF, token);
                                token = strtok(NULL, " ");
                                if(token != NULL){
                                    printf("Too many arguments\n");
                                    ret_val = false;
                                }
                            }
                        }
                    }
                }
                break;
            case 'D':
                msg_info->opcode = DELETE;
                token = strtok(NULL, " ");
                if((check_desIP_valid(token) == false) || (token == NULL)){
                    ret_val = false;
                }
                else{
                    strcpy(msg_info->data.destination, token);
                    token = strtok(NULL, " ");
                    if((check_mask_valid(token) == false) || (token == NULL)){
                        ret_val = false;
                    }
                    else{
                        strcpy(msg_info->data.mask, token);
                        token = strtok(NULL, " ");
                        if(token != NULL){
                            printf("Too many arguments\n");
                            ret_val = false;
                        }
                    }
                }
                break;
            case 'S':
                msg_info->opcode = SHOW;
                token = strtok(NULL, " ");
                if(token == NULL){

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