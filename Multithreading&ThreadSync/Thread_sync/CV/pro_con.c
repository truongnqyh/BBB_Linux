#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include "queue.h"
#include "assert.h"

struct Queue_t *queue;
static const char *pro1 = "TP1";
static const char *pro2 = "TP2";
static const char *pro3 = "TP3";
static const char *con1 = "TC1";
static const char *con2 = "TC2";
static const char *con3 = "TC3";

int new_item(){
    static int item = 0;
    item++;
    return item;
}

static void *pro_fc_callback(void *arg){
    int item;
    char *name = (char *)arg;

    printf("Thread %s is waiting to lock the queue\n", name);
    pthread_mutex_lock(&queue->q_mutex);
    printf("Thread %s locked the queue\n", name);
    while(is_queue_full(queue)){
        printf("Thread %s blocks itself, queue is full \n", name);
        pthread_cond_wait(&queue->q_cv, &queue->q_mutex);
        printf("Thread %s wakes up, check the queue again \n", name);
    }
    assert(is_queue_empty(queue));
    while(!is_queue_full(queue)){
        item = new_item();
        printf("Thread %s is producing new item: %d \n", name, item);
        enqueue(queue, (void *)item);
        printf("Thread %s push item %d to queue, queue size is %d \n", name, item, queue->count);
    }
    printf("Thread %s filled up the queue, signalling and releasing lock \n", name);
    pthread_cond_broadcast(&queue->q_cv);
    pthread_mutex_unlock(&queue->q_mutex);
    printf("Thread %s finished and exit\n", name);
    
    return NULL;
}

static void *con_fc_callback(void *arg){
    int item;
    char *name = (char *)arg;

    printf("Thread %s is waiting to lock the queue\n", name);
    pthread_mutex_lock(&queue->q_mutex);
    printf("Thread %s locked the queue\n", name);
    while(is_queue_empty(queue)){
        printf("Thread %s blocks itself, queue is empty \n", name);
        pthread_cond_wait(&queue->q_cv, &queue->q_mutex);
        printf("Thread %s wakes up, check the queue again \n", name);
    }
    assert(is_queue_full(queue));
    while(!is_queue_empty(queue)){
        item = (int)deque(queue);
        printf("Thread %s is consuming new item: %d \n", name, item);
        printf("Thread %s clear item %d from queue, queue size is %d \n", name, item, queue->count);
    }
    printf("Thread %s drains the entire queue, signalling and releasing lock \n", name);
    pthread_cond_broadcast(&queue->q_cv);
    pthread_mutex_unlock(&queue->q_mutex);
    printf("Thread %s finished and exit\n", name);
    
    return NULL;
}

int main(int argc, char **argv){
    pthread_attr_t attr;
    pthread_t producer1, producer2, producer3, consumer1, consumer2, consumer3;
    
    queue = initQ();
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    pthread_create(&producer1, &attr, pro_fc_callback, (void *)pro1);
    pthread_create(&producer2, &attr, pro_fc_callback, (void *)pro2);
    pthread_create(&producer3, &attr, pro_fc_callback, (void *)pro3);
    pthread_create(&consumer1, &attr, con_fc_callback, (void *)con1);
    pthread_create(&consumer2, &attr, con_fc_callback, (void *)con2);
    pthread_create(&consumer3, &attr, con_fc_callback, (void *)con3);


    pthread_join(producer1, 0);
    pthread_join(producer2, 0);
    pthread_join(producer3, 0);
    pthread_join(consumer1, 0);
    pthread_join(consumer2, 0);
    pthread_join(consumer3, 0);

    printf("No of queue(should be 0): %d\n", queue->count);
    printf("Program finish with success\n");

    return 0;
}