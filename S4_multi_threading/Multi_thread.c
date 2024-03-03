#include <stdio.h>
#include <pthread.h>

/* This's main thread func */

/* Using only one variable sum will raise error
int sum = 0; -> wrong 
volatile sum = 0; -> wrong
*/
int sum1 = 0;
int sum2 = 2;
pthread_mutex_t sum1_lock;
void *sum_1(void *arg)
{
    int i = 0;

    for( i = 0; i < 50000; i++)
    {
        pthread_mutex_lock(&sum1_lock);
        sum1 += i;
        pthread_mutex_unlock(&sum1_lock);
    }

}

void *sum_2(void *arg)
{
    int i = 0;

    for( i = 50000; i < 100000; i++)
    {
        pthread_mutex_lock(&sum1_lock);
        sum2 += i;
        pthread_mutex_unlock(&sum1_lock);
    }

}

int main()
{
    int sum = 0;
    int i = 0;
    pthread_t pt1;
    pthread_t pt2;

    pthread_mutex_init(&sum1_lock, NULL);
    pthread_create(&pt1, NULL, sum_1, NULL);
    pthread_create(&pt2, NULL, sum_2, NULL);
    pthread_join(pt1, NULL);
    pthread_join(pt1, NULL);
    pthread_mutex_destroy(&sum1_lock, NULL);


    printf("caculated sum : %d\n", sum1 + sum2);
    for( i = 50000; i < 100000; i++)
    {
        sum += i;
    }
    printf("real sum : %d\n", sum);
}