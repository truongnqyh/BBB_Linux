#include "common_mac.h"
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

static mac_table_list_struct_t create_new_entrydir_node(const mac_table_struct_t inputEntryInfo);
static mac_table_struct_t create_new_info(char mac[MAC_ADDR_LEN]);
static mac_table_list_struct_t add_entry_dir(mac_table_list_struct_t head, char mac[MAC_ADDR_LEN]);


static mac_table_list_struct_t add_entry_dir(mac_table_list_struct_t head, char mac[MAC_ADDR_LEN])
{
    mac_table_struct_t entryInfo = NULL;

    entryInfo = create_new_info(mac);
    head = add_last_entry_list_mac(head, entryInfo);

    return head;
}

mac_table_list_struct_t init_head_mac(mac_table_list_struct_t head)
{
    head = (mac_table_list_struct_t)malloc(sizeof(struct mac_table_list_struct));
    head = NULL;
    return head;
}

static mac_table_list_struct_t create_new_entrydir_node(const mac_table_struct_t inputEntryInfo)
{
    mac_table_list_struct_t temp = NULL; // declare a node

    temp = (mac_table_list_struct_t)malloc(sizeof(struct mac_table_list_struct));
    temp->next = NULL;
    temp->mac_info = inputEntryInfo;

    return temp;
}

mac_table_list_struct_t add_last_entry_list_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo)
{
    mac_table_list_struct_t temp;
    mac_table_list_struct_t ptr;

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

mac_table_list_struct_t update_entry_table_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo)
{
    mac_table_list_struct_t ptr;

    ptr = head; 
    while (ptr != NULL)
    {
        if(strncmp(ptr->mac_info->mac, inputEntryInfo->mac, MAC_ADDR_LEN - 1) == 0){
            /* Update routing table */
            memcpy(ptr->mac_info->mac, inputEntryInfo->mac, MAC_ADDR_LEN);
        }
        ptr = ptr->next; /* last node->next = NULL*/
    }
    
    return head;
}

mac_table_list_struct_t remove_node_list_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo)
{
    mac_table_list_struct_t ptr;
    mac_table_list_struct_t last_ptr;
    int count = 0;

    ptr = head;
    while (ptr != NULL){
        if(strcmp(ptr->mac_info->mac, inputEntryInfo->mac) == 0){
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
            free(ptr->mac_info);
            free(ptr);
            break;
        }
        count ++;
        last_ptr = ptr; /* last node */
        ptr = ptr->next; /* last node -> next */
    }

    return head;
}

static mac_table_struct_t create_new_info(char mac[MAC_ADDR_LEN])
{
    mac_table_struct_t newEntryInfo = NULL;

    newEntryInfo = (mac_table_struct_t)malloc(sizeof(struct mac_table_struct));
    if(newEntryInfo == NULL){
        printf("Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memcpy(newEntryInfo, mac, MAC_ADDR_LEN);

    return newEntryInfo;
}

bool check_condition_mac(mac_table_list_struct_t head, char mac[MAC_ADDR_LEN], FUNCTION opcode)
{
    mac_table_list_struct_t ptr;
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
                if(strncmp(ptr->mac_info->mac, mac, MAC_ADDR_LEN - 1) == 0){
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
                while (ptr != NULL)
                {
                    if(strncmp(ptr->mac_info->mac, mac, MAC_ADDR_LEN - 1) == 0){
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
                    if(strncmp(ptr->mac_info->mac, mac, MAC_ADDR_LEN - 1) == 0){
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

void show_mac_info(mac_table_list_struct_t head)
{
    mac_table_list_struct_t ptr;
    ptr = head;
    char ip_value[IP_ADDR_LEN];
    char copy_ip_value[IP_ADDR_LEN];

    if(NULL == ptr){
        printf("------> The mac table is empty, there's nothing to show\n");
    }
    else{ 
        printf("%-24s%-24s\n","MAC address           |","IP Shm                 ");
        printf("%-24s%-24s\n","----------------------|","----------------------|");
        while (ptr != NULL)
        {
            memset(ip_value, 0, IP_ADDR_LEN);
            get_IP_from_Shm(ptr->mac_info->mac, ip_value);
            memcpy(copy_ip_value, ip_value, strlen(ip_value) - 1);
            printf("%-22s| %-22s|\n", ptr->mac_info->mac, copy_ip_value);
            ptr = ptr->next;
        }
    }
}

mac_table_list_struct_t flush_mac_info(mac_table_list_struct_t head)
{
    mac_table_list_struct_t ptr;
    mac_table_list_struct_t last_ptr;

    ptr = head;
    while (ptr != NULL)
    {  
        last_ptr = ptr;
        ptr = ptr -> next;
        free(last_ptr->mac_info);
        free(last_ptr);
    }
    head = NULL;

    return head;
}

void send_mac_table_to_newly_client(int fd, mac_table_list_struct_t head){
    mac_table_list_struct_t ptr;
    char buffer[128];

    ptr = head;
    while(ptr != NULL)
    {
        sprintf(buffer, "C %s\n", \
        ptr->mac_info->mac);
        write(fd, buffer, 128);
        ptr = ptr->next;
    }
}

bool store_IP_to_Shm(char *mac, char ip[IP_ADDR_LEN])
{
    int shm_fd;

    shm_fd = shm_open(mac, O_CREAT | O_RDWR | O_TRUNC, 0660);
    if (shm_fd == -1) {
        printf("Couldn't create shared memory for MAC addr: %s and IP value: %s \n", mac, ip);
        return false;
    }
    if(ftruncate(shm_fd, IP_ADDR_LEN) == -1){
        printf("Couldn't truncate shared memory for MAC addr: %s and IP value: %s \n", mac, ip);
        return false;
    }
    void *shm_ptr = mmap(NULL, IP_ADDR_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED){
        printf("mapping failed\n");
        return false;
    }
    memset(shm_ptr, 0, IP_ADDR_LEN);
    /* Copy ip value to shared memory */
    memcpy(shm_ptr, ip, IP_ADDR_LEN);
    if(munmap(shm_ptr, IP_ADDR_LEN) == -1){
        printf("munmap failed\n");
        return false;
    }
    close(shm_fd);

    return true;
}

bool get_IP_from_Shm(char *mac, char ip[IP_ADDR_LEN])
{
    int shm_fd;

    shm_fd = shm_open(mac, O_CREAT | O_RDONLY, 0660);
    if (shm_fd == -1) {
        printf("Couldn't create shared memory for MAC addr: %s and IP value: %s \n", mac, ip);
        return false;
    }
    void *shm_ptr = mmap(NULL, IP_ADDR_LEN, PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED){
        printf("mapping failed\n");
        return false;
    }
    /* Copy value from shared memory to ip buffer */
    memcpy(ip, shm_ptr, IP_ADDR_LEN);
    if(munmap(shm_ptr, IP_ADDR_LEN) == -1){
        printf("munmap failed\n");
        return false;
    }
    close(shm_fd);

    return true;
}

bool update_IP_in_Shm(char *mac, char ip[IP_ADDR_LEN])
{
    int shm_fd;

    shm_fd = shm_open(mac, O_CREAT | O_RDWR | O_TRUNC, 0660);
    if (shm_fd == -1) {
        printf("Couldn't create shared memory for MAC addr: %s and IP value: %s \n", mac, ip);
        return false;
    }
    if(ftruncate(shm_fd, IP_ADDR_LEN) == -1){
        printf("Couldn't truncate shared memory for MAC addr: %s and IP value: %s \n", mac, ip);
        return false;
    }
    void *shm_ptr = mmap(NULL, IP_ADDR_LEN, PROT_WRITE | PROT_READ, MAP_SHARED, shm_fd, 0);
    if(shm_ptr == MAP_FAILED){
        printf("mapping failed\n");
        return false;
    }
    memset(shm_ptr, 0, IP_ADDR_LEN);
    /* Copy ip value to shared memory */
    memcpy(shm_ptr, ip, IP_ADDR_LEN);
    if(munmap(shm_ptr, IP_ADDR_LEN) == -1){
        printf("munmap failed\n");
        return false;
    }
    close(shm_fd);

    return true;
}


bool check_format_MAC_address(char mac[MAC_ADDR_LEN]){
    int i;
    bool ret_val = true;

    for(i = 0; i < MAC_ADDR_LEN - 1; i++) {
        if(((i % 3) != 2) && (!isxdigit(mac[i]))){
            ret_val = false;
        }
        if(((i % 3) == 2) && (mac[i] != ':')){
            ret_val = false;
        }
    }
    if (mac[i] != '\n'){
        ret_val = false;
    }

    return ret_val;
}


