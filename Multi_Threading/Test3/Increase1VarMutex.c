#include <stdio.h>
#include <pthread.h>

/* This's main thread func */

/* Using only one variable sum will raise error, race condition -> need to use 2 seperated variable or use lock (mutex, smp, ...)
int sum = 0; -> wrong 
volatile sum = 0; -> wrong
*/
int variable = 0;
int count1 = 0;
int count2 = 0;
pthread_mutex_t sum1_lock;

void *Increase_1(void *arg)
{
    count1 += 1;
    pthread_mutex_lock(&sum1_lock);
    variable += 1;
    pthread_mutex_unlock(&sum1_lock);
}

void *Increase_2(void *arg)
{
    count2 += 1;
    pthread_mutex_lock(&sum1_lock);
    variable += 1;
    pthread_mutex_unlock(&sum1_lock);
}

int main()
{
    int sum = 0;
    int i = 0;
    pthread_t pt1;
    pthread_t pt2;

    /* Create mutex */
    pthread_mutex_init(&sum1_lock, NULL);
    for (int i = 0; i < 100; i++)
    {
        pthread_create(&pt1, NULL, &Increase_1, NULL);
        pthread_create(&pt2, NULL, &Increase_2, NULL);
        pthread_join(pt1, NULL);
        pthread_join(pt2, NULL);
    }
    /* Destroy mutex */
    pthread_mutex_destroy(&sum1_lock);
    printf("Value of var : %d\n", variable);
    printf("Count of calculation : %d\n", count1 + count2);

    return 0;
}