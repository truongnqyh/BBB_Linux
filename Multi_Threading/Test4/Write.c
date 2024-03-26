#include <stdio.h>
#include <pthread.h>
#include <Write.h>

int fd = 0;
pthread_mutex_t file_lock;

void *Hello(int *fd)
{
        char *buffer = "Hello";
        ssize_t write_fd = 0;

        pthread_mutex_lock(&file_lock);
        write_fd = write(*fd, buffer, 5);
        pthread_mutex_unlock(&file_lock);
}

void *World(int *fd)
{
        char *buffer = "World";
        ssize_t write_fd = 0;

        pthread_mutex_lock(&file_lock);
        write_fd = write(*fd, buffer, 5);
        pthread_mutex_unlock(&file_lock);
}

void init_sum_lib(int *fd)
{
    pthread_mutex_init(&file_lock, NULL);
    *fd = 0;
}

void free_sum_lib()
{
    pthread_mutex_destroy(&file_lock);
}