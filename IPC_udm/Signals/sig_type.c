#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static void ctrlC_sig_handler(int sig)
{
    printf("Ctrl_C pressed\n");
    exit(0);
}

static void abort_sig_handler(int sig)
{
    printf("Abort signal raised\n");
    exit(0);
}

int main(int argc, char **argv){
    char input;
    int ret;

    ret = signal(SIGINT, ctrlC_sig_handler);
    if(ret == SIG_ERR){
        printf("signal(SIGINT) failed\n");
        exit(EXIT_FAILURE);
    }
    signal(SIGABRT, abort_sig_handler);
    printf("Want to abort? y/n\n");
    scanf("%c", &input);
    if(input == 'y'){
        /* 2 ways to abort: call abort() system call or use raise() */
        /* abort(); */ 
        raise(SIGABRT);
    }

    return 0;
}


