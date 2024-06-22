#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>

#define ARR_SIZE 5
int array[] = {1, 2, 3, 4, 5};
pthread_mutex_t arr_mutex;

static void *thread_fn_callback_sum(void *arg){
    int i;
    int sum;

    do{
        i = 0;
        sum = 0;
        pthread_mutex_lock(&arr_mutex);
        while(i < ARR_SIZE){
            sum += array[i];
            i++;
        }
        printf("Sum = %d\n", sum);
        pthread_mutex_unlock(&arr_mutex);
    }while(1);
}

static void *thread_fn_callback_swap(void *arg){
    int temp;

    do{
        pthread_mutex_lock(&arr_mutex);
        temp = array[0];
        array[0] = array[ARR_SIZE - 1];
        array[ARR_SIZE - 1] = temp;
        pthread_mutex_unlock(&arr_mutex);
    }while(1);
}

void create_thread_sum(){
    pthread_t pthread1;
    int rc;

    rc = pthread_create(&pthread1, NULL, thread_fn_callback_sum, NULL);
    if(rc != 0)
    {
        printf("Create sum thread fail with errno: %d\n", rc);
        exit(0);
    }
}

void create_thread_swap(){
    pthread_t pthread2;
    int rc;

    rc = pthread_create(&pthread2, NULL, thread_fn_callback_swap, NULL);
    if(rc != 0)
    {
        printf("Create swap thread fail with errno: %d\n", rc);
        exit(0);
    }
}

int main(int argc, char **argv){
    pthread_mutex_init(&arr_mutex, NULL);
    create_thread_sum();
    create_thread_swap();
    pthread_mutex_destroy(&arr_mutex);
    pthread_exit(0);

    return 0;
}
