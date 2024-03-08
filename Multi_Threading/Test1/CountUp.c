#include <stdio.h>
#include <pthread.h>
#include <time.h>

__int64_t sum, sum1, sum2;

void *sum_1(void *arg)
{
    __int64_t i = 0;

    for( i = 0; i < 50000000000; i++)
    {
        sum1 += i;
    }
}

void *sum_2(void *arg)
{
    __int64_t i = 0;

    for( i = 50000000000; i < 100000000000; i++)
    {
        sum2 += i;
    }
}

int main()
{
    __int64_t sum = 0;
    __int64_t i = 0;
    clock_t begin;
    clock_t end;
    pthread_t pt1;
    pthread_t pt2;

    pthread_create(&pt1, NULL, sum_1, NULL);
    pthread_create(&pt2, NULL, sum_2, NULL);
    begin = clock();
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);
    end = clock();
    printf("Time run : %f\n", (float)(end-begin)/CLOCKS_PER_SEC);
    printf("Caculated sum : %ld\n", sum1 + sum2);

    begin = clock();
    for( i = 0; i < 100000000000; i++)
    {
        sum += i;
    }
    end = clock();
    printf("Time run : %f\n", (float)(end-begin)/CLOCKS_PER_SEC);
    printf("real sum : %ld\n", sum);
}