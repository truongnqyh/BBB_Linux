#define DEST_SIZE 16
#define GATEW_SIZE 16
#define MASK_SIZE 3
#define OIF_SIZE 16
typedef enum{
    CREATE,
    UPDATE,
    DELETE,
    SHOW,
    FLUSH
}FUNCTION;

typedef struct data_info{
    char destination[DEST_SIZE];
    char mask[MASK_SIZE];
    char gateway_ip[GATEW_SIZE];
    char OIF[OIF_SIZE];
}data_info_t;

typedef struct msg{
    FUNCTION opcode;
    data_info_t data;
}msg_t;


/* strtok */