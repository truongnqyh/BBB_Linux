#define DEST_SIZE 16
#define GATEW_SIZE 16
typedef enum{
    CREATE,
    UPDATE,
    DELETE,
    SHOW,
    FLUSH
}FUNCTION;

typedef struct data_info{
    char destination[DEST_SIZE];
    char mask;
    char gateway_ip[GATEW_SIZE];
    char OIF;
}data_info_t;

typedef struct msg{
    FUNCTION opcode;
    data_info_t data;
}msg_t;


/* strtok */