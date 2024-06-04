#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <stdbool.h>
#include "types.h"
#include <ctype.h>

/* Mac addr example: XX:XX:XX:XX:XX:XX */
/* Ip addr example: X.X.X.X */
#define MAC_ADDR_LEN 18
#define IP_ADDR_LEN 16
/**************************************************************/
struct mac_table_struct
{
    char mac[MAC_ADDR_LEN];
};
typedef struct mac_table_struct *mac_table_struct_t;

struct mac_table_list_struct
{
    mac_table_struct_t mac_info;
    struct mac_table_list_struct *next;
};
typedef struct mac_table_list_struct *mac_table_list_struct_t;
/**************************************************************/

/******************* Linked list APIs **********************/
mac_table_list_struct_t add_last_entry_list_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo);
mac_table_list_struct_t remove_node_list_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo);
mac_table_list_struct_t update_entry_table_mac(mac_table_list_struct_t head, mac_table_struct_t inputEntryInfo);
void show_mac_info(mac_table_list_struct_t head);
mac_table_list_struct_t init_head_mac(mac_table_list_struct_t head);
mac_table_list_struct_t flush_mac_info(mac_table_list_struct_t head);
bool check_condition_mac(mac_table_list_struct_t head, char mac[MAC_ADDR_LEN], FUNCTION opcode);
void send_mac_table_to_newly_client(int fd, mac_table_list_struct_t head);
bool store_IP_to_Shm(char *mac, char ip[IP_ADDR_LEN]);
bool get_IP_from_Shm(char *mac, char ip[IP_ADDR_LEN]);
bool update_IP_in_Shm(char *mac, char ip[IP_ADDR_LEN]);
bool check_format_MAC_address(char mac[MAC_ADDR_LEN]);