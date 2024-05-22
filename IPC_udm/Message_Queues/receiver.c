#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/un.h>
#include <sys/socket.h>

#define MGS_SIZE_MAX 256
#define MSG_BUFFER_SIZE (MGS_SIZE_MAX + 10)
#define PERMISSION 0660

int main(int argc, char *argv[]){
    fd_set read_fd;
    char buffer[MSG_BUFFER_SIZE];
    int msgq_fd;
    struct mq_attr attr;

    /* O_NONBLOCK -> will crash imidiately 
    * 0 -> waiting for data in MsgQ */
    attr.mq_flags = 0; 
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MGS_SIZE_MAX;
    attr.mq_curmsgs = 0;
    /* Check whether user suppy MsgQ name from command-line on terminal */
    if(argc <= 1){
        printf("lack of arguments for MsgQ name -> argv[1] \n");
        exit(EXIT_FAILURE);
    }
    /* Create MsgQ */
    if((msgq_fd = mq_open(argv[1], O_RDWR | O_CREAT, PERMISSION, &attr)) == -1){
        printf("mq_open fail-> %d\n", errno);
        exit(EXIT_FAILURE);
    }
    printf("Open MsgQ successfully\n"); 
    while(1){
        FD_ZERO(&read_fd);
        FD_SET(msgq_fd, &read_fd);
        printf("Waiting for data from MsgQ..... -> blocking on select()\n");
        /* Wait for data from MsgQ, using select() for multiple senders */
        select(msgq_fd + 1, &read_fd, NULL, NULL, NULL);
        if(FD_ISSET(msgq_fd, &read_fd)){
            /* Clear buffer */
            memset(buffer, 0, MSG_BUFFER_SIZE);
            /* Receive data from MsgQ */
            if(mq_receive(msgq_fd, buffer, MSG_BUFFER_SIZE, NULL) == -1){
                printf("mq_receive fail-> %d\n", errno);
                exit(EXIT_FAILURE);
            }
            printf("Data received from MsgQ: %s\n", buffer);
        }
        
        // /* Clear buffer */
        // memset(buffer, 0, MSG_BUFFER_SIZE);
        // /* Receive data from MsgQ */
        // printf("Dequeueing data from MsgQ \n");
        // if(mq_receive(msgq_fd, buffer, MSG_BUFFER_SIZE, NULL) == -1){
        //     printf("mq_receive fail-> %d\n", errno);
        //     exit(EXIT_FAILURE);
        // }
        // printf("Data received from MsgQ: %s\n", buffer); 
    }
    mq_close(msgq_fd);
    
    return 0;
    
}