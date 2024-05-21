#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
    int pid = 0;

    pid = atoi(argv[1]);
    if(0 != kill(pid, SIGINT))
    {
        printf("cannot send signal\n");
    }

    return 0;
}
