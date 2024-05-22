#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#define MGS_SIZE_MAX 256
#define MSG_BUFFER_SIZE (MGS_SIZE_MAX + 10)
#define PERMISSION 0660
int main(int argc, char *argv[]){
    char buffer[MSG_BUFFER_SIZE];
    int msgq_fd;

    /* Check whether user suppy MsgQ name from command-line on terminal */
    if(argc <= 1){
        printf("lack of arguments for MsgQ name -> argv[1] \n");
        exit(EXIT_FAILURE);
    }
    /* Clear buffer */
    memset(buffer, 0, MSG_BUFFER_SIZE);
    /* Enter data to send to MsgQ */
    printf("Enter the data to send to MsgQ %s\n", argv[1]);
    scanf("%s", &buffer);
    /* Create MsgQ */
    if((msgq_fd = mq_open(argv[1], O_RDWR | O_CREAT, 0, 0)) == -1){
        printf("mq_open fail-> %d\n", errno);
        exit(EXIT_FAILURE);
    }
    /* Send data to MsgQ */
    if(mq_send(msgq_fd, buffer, strlen(buffer), 0) == -1){
        printf("mq_send fail-> %d\n", errno);
        exit(EXIT_FAILURE);
    }
    mq_close(msgq_fd);
    
    return 0;
}
