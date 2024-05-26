#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* getpid() */

int main(int argc, char **argv){
    kill(10831 ,SIGUSR1);

    return 0;
}


