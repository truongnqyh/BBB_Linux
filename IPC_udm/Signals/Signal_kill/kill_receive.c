#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void sigusr1_handler(int sig)
{
    printf("Receive signal from other process\n");
    exit(0);
}

int main(int argc, char **argv){
    char input;
    int ret;

    ret = signal(SIGUSR1, sigusr1_handler);
    if(ret == SIG_ERR){
        printf("signal(SIGUSR1) failed\n");
        exit(EXIT_FAILURE);
    }
    while(1);

    return 0;
}


