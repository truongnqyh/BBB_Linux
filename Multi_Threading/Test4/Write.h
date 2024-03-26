#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

#define linkconf "/home/truong/workspace/Linux_Aphu/BBB_Linux/Multi_Threading/Test3/test.txt"

void *Hello(int *fd);
void *World(int *fd);
void init_sum_lib(int *fd);
void free_sum_lib();
