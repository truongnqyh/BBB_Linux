#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

static void *thread1_callback(void *arg){
    char *buffer = (char *)arg;

    while(1){
        printf("%s\n", buffer);
        sleep(1);
    }
}

void thread1_create(){
    /* buffer has to be saved not in stack -> out of this function this variable will be clear */
    static char *buffer = "This is thread 1";
    pthread_t thread1;
    int ret_val;

    ret_val = pthread_create(&thread1, NULL, thread1_callback, (void *)buffer);
    if(ret_val != 0){
        printf("Could not create thread, errno = %d\n", ret_val);
        exit(0);
    }
}

int main(){
    thread1_create();
    printf("Main pause\n");
    pause();

    return 1;
}