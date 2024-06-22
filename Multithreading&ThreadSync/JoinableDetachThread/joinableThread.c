#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

pthread_t pthread1;
pthread_t pthread2;

void *thread_callback(void *arg){
    int number = 0;
    int count = 0;

    number = *(int *)arg; 
    free(arg);
    while (count != number){
        printf("Thread %d is doing\n", number);
        sleep(1);
        count++;
    }
    int *result = calloc(1, sizeof(int));
    *result = number * number;

    return (void *)result;
}
void thread_create(pthread_t *pthread, int number){
    pthread_attr_t attr;
    int *squared_num = calloc(1, sizeof(int));
    *squared_num = number;
    /* PTHREAD_CREATE_DETACHED */
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(pthread, &attr, thread_callback, (void *)squared_num);
}
int main(int argc, char **argv){
    void *result1;
    void *result2;

    thread_create(&pthread1, 3);
    thread_create(&pthread2, 8);
    printf("main fnt's blocking on pthread join for thread with id = 1\n");
    pthread_join(pthread1, &result1);
    if(result1){
        printf("return result from thread 2 = %d\n", *(int *)result1);
        free(result1);
    }
    printf("main fnt's blocking on pthread join for thread with id = 2\n");
    pthread_join(pthread2, &result2);
    if(result2){
        printf("return result from thread 2 = %d\n", *(int *)result2);
        free(result2);
    }

    return 0;
}