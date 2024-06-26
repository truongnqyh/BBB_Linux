#include "common_mac.h"
/**************************************************************/
struct routing_table_struct
{
    data_info_t msg;
};
typedef struct routing_table_struct *routing_table_struct_t;

struct routing_table_list_struct
{
    routing_table_struct_t msg_info;
    struct routing_table_list_struct *next;
};
typedef struct routing_table_list_struct *routing_table_list_struct_t;
/**************************************************************/
#define NO_DATA 'NULL'
#define SOCKET_NAME "socketdemo/unixsocket_test"
#define BUFFER_SIZE 128
#define MAX_CLIENTS_SUPPORT 32
#define MAX_DESTINATION 10
#define IS_RT 0
#define IS_MAC 1
#define EXIT_MSG 4

extern int monitor_fd_set[MAX_CLIENTS_SUPPORT];
extern int client_pid_set[MAX_CLIENTS_SUPPORT];
/********************** Socket APIs *************************/
void remove_from_monitor_fd_set(int fd);
void add_to_monitor_fd_set(int fd);
void init_monitor_fd_set();
void clone_to_fd_set(fd_set *fd_set);
int max_fd();
void init_client_fd_set();
void add_to_pid_set(pid_t pid);
void remove_from_client_pid_set(pid_t pid);
bool check_available_pid(pid_t pid);
/******************* Linked list APIs **********************/
routing_table_list_struct_t add_last_entry_list(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo);
routing_table_list_struct_t remove_node_list(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo);
routing_table_list_struct_t update_entry_table(routing_table_list_struct_t head, routing_table_struct_t inputEntryInfo);
void show_routing_table_info(routing_table_list_struct_t head);
routing_table_list_struct_t init_head(routing_table_list_struct_t head);
routing_table_list_struct_t flush_routing_table_info(routing_table_list_struct_t head);
bool check_condition(routing_table_list_struct_t head, data_info_t msg_info, FUNCTION opcode);
void send_rtable_to_newly_client(int fd, routing_table_list_struct_t head);
/**********************************************************/
bool check_format_input_string(char input_string[BUFFER_SIZE], msg_t *msg_info, mac_table_struct_t mac_addr, int* is_RT_MAC);
bool check_desIP_valid(char* dest);
bool check_mask_valid(char* mask, int size);
bool check_gateway_valid(char* gateway);
bool check_OIF_valid(char* oif);