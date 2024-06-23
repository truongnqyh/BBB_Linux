#include "dnphil.h"

#define NO_PHIL 5
static philer_t philer[NO_PHIL];
static spoon_t spoon[NO_PHIL];

static spoon_t *phil_get_left_spoon(philer_t *phil);
static spoon_t *phil_get_right_spoon(philer_t *phil);
static void phil_eat(philer_t *phil);
static bool phil_access_both_spoon(philer_t *phil);
static void phil_release_both_spoon(philer_t *phil);
static void *philosopher_fn_callback(void *arg);

int main(int argc, char **argv){
    int i;
    pthread_attr_t attr;

    /* Init spoon array */
    for (i = 0; i < NO_PHIL; i++) {
        spoon[i].spoon_id = i;
        spoon[i].is_used = false;
        spoon[i].phil = NULL;
        pthread_mutex_init(&spoon[i].mutex, NULL);
        pthread_cond_init(&spoon[i].cv, NULL);
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    /* init philosopher array */
    for (i = 0; i < NO_PHIL; i++) {
        philer[i].phil_id = i;
        philer[i].eat_count = 0;
        pthread_create(&philer[i].thread_handle, &attr, philosopher_fn_callback, (void *)&philer[i]);
    }

    pthread_exit(0);
    return 0;
}

static void *philosopher_fn_callback(void *arg){
    philer_t *phil = (philer_t *)arg;

    while(1){
        if(phil_access_both_spoon(phil) == true)
        {
            phil_eat(phil);
            phil_release_both_spoon(phil);
            sleep(1);
        }
    }

    return NULL;
}

static spoon_t *phil_get_left_spoon(philer_t *phil){
    return &spoon[phil->phil_id];
}

static spoon_t *phil_get_right_spoon(philer_t *phil){
    int philer_id;

    philer_id = phil->phil_id;
    if(philer_id == 0){
        return &spoon[NO_PHIL - 1];
    }

    return &spoon[philer_id - 1];
}

static void phil_eat(philer_t *phil){
    spoon_t *left_spoon;
    spoon_t *right_spoon;

    left_spoon = phil_get_left_spoon(phil);
    right_spoon = phil_get_right_spoon(phil);
    assert(left_spoon->is_used == true);
    assert(left_spoon->phil == phil);
    assert(right_spoon->is_used == true);
    assert(right_spoon->phil == phil);
    phil->eat_count++;
    printf("Phil %d is eating with spoons (%d,%d) and eat_count : %d\n", \
    phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id, phil->eat_count);
    sleep(1);
}

static bool phil_access_both_spoon(philer_t *phil){
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    /* Firstly check the left spoon is free or not */
    printf("Phil %d is trying to access and waiting for lock left spoon %d \n", phil->phil_id, left_spoon->spoon_id);
    pthread_mutex_lock(&left_spoon->mutex);
    printf("Phil %d locked left spoon %d \n", phil->phil_id, left_spoon->spoon_id);
    while((left_spoon->is_used == true) && (left_spoon->phil != phil)){
        printf("Phil %d is waiting for left_spoon %d is free \n", phil->phil_id, left_spoon->spoon_id);
        pthread_cond_wait(&left_spoon->cv, &left_spoon->mutex);
        printf("Phil %d received signal left_spoon %d is free \n", phil->phil_id, left_spoon->spoon_id);
    }
    left_spoon->is_used = true;
    left_spoon->phil = phil;
    pthread_mutex_unlock(&left_spoon->mutex);
    printf("Phil %d is now trying to access and waiting for lock right spoon %d \n", phil->phil_id, right_spoon->spoon_id);
    pthread_mutex_lock(&right_spoon->mutex);
    printf("Phil %d locked right spoon %d \n", phil->phil_id, right_spoon->spoon_id);
    /* Then check the right spoon is free or not */
    if(right_spoon->is_used == false){
        printf("Phil %d got both spoons (%d,%d) \n", phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id);
        right_spoon->is_used = true;
        right_spoon->phil = phil;
        pthread_mutex_unlock(&right_spoon->mutex);
        return true;
    }
    else{
        if(right_spoon->phil != phil){
            /* Right spoon is being used by another phisolopher */
            printf("Phil %d cannot access right spoon %d because another philosopher is using\n", phil->phil_id, right_spoon->spoon_id);
            printf("Phil %d is releasing left spoon %d then \n", phil->phil_id, left_spoon->spoon_id);
            pthread_mutex_lock(&left_spoon->mutex);
            assert(left_spoon->is_used == true);
            assert(left_spoon->phil == phil);
            left_spoon->is_used = false;
            left_spoon->phil = NULL;
            printf("Phil %d released left spoon %d \n", phil->phil_id, left_spoon->spoon_id);
            pthread_mutex_unlock(&left_spoon->mutex);
            pthread_mutex_unlock(&right_spoon->mutex);
            return false;
        }
        else{
            /* Right spoon is being used by current phisolopher */
            printf("Phil %d got both spoons (%d,%d) \n", phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id);
            pthread_mutex_unlock(&right_spoon->mutex);
            return true;
        }
    }

    assert(0);
    return false;
}

static void phil_release_both_spoon(philer_t *phil){
    spoon_t *left_spoon = phil_get_left_spoon(phil);
    spoon_t *right_spoon = phil_get_right_spoon(phil);

    printf("Phil %d is trying to release and waiting for lock  their spoons \n", phil->phil_id);
    pthread_mutex_lock(&left_spoon->mutex);
    pthread_mutex_lock(&right_spoon->mutex);
    printf("Phil %d locked spoons (%d,%d)\n", phil->phil_id, left_spoon->spoon_id, right_spoon->spoon_id);
    assert(left_spoon->is_used == true);
    assert(left_spoon->phil == phil);
    assert(right_spoon->is_used == true);
    assert(right_spoon->phil == phil);
    /* Release left spoon */
    left_spoon->is_used = false;
    left_spoon->phil = NULL;
    pthread_cond_signal(&left_spoon->cv);
    pthread_mutex_unlock(&left_spoon->mutex);
    printf("Phil %d signaled, released and unlocked left spoon %d\n", phil->phil_id, left_spoon->spoon_id);
    /* Release right spoon */
    right_spoon->is_used = false;
    right_spoon->phil = NULL;
    pthread_cond_signal(&right_spoon->cv);
    pthread_mutex_unlock(&right_spoon->mutex);
    printf("Phil %d signaled, released and unlocked right spoon %d\n", phil->phil_id, right_spoon->spoon_id);
    printf("Phil %d releases successfully \n", phil->phil_id);
}