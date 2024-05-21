#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

void signal_handler(int signo)
{
    static int i = 0;

    if(signo == SIGINT)
    {
        printf("receive signal\n");
        i += 1;
        printf("receving times: %d\n", i);
    }
}
int main()
{
    if(signal(SIGINT, signal_handler) == SIG_ERR)
    {
        printf("invalid SIGINT\n");
    }
    while(1)
        sleep(1);

    return 0;
}