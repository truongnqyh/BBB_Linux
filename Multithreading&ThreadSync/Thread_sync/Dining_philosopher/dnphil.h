#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <stdbool.h>

typedef struct philer{
    int phil_id;
    pthread_t thread_handle;
    int eat_count;
}philer_t;

typedef struct spoon{
    int spoon_id;
    bool is_used;
    philer_t *phil;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
}spoon_t;

