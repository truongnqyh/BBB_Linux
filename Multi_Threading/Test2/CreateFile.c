#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define LINKMT "/home/truong/workspace/Linux_Aphu/BBB_Linux/Multi_Threading/Test2/MThread/file%d"
#define LINKNOMT "/home/truong/workspace/Linux_Aphu/BBB_Linux/Multi_Threading/Test2/NoMThread/file%d"

typedef enum Common_types
{
	TEN_FILE,
	ZERO_TO_FIVE_FILE,
	FIVE_TO_TEN_FILE
}Types;

int randomly(int minN, int maxN);
void WriteNumberToFile(int fd, int number);
void Create_File(Types Type);

int main()
{
    int number = 0;
    int index = 0;
    clock_t begin;
    clock_t end;
    pthread_t pt1;
    pthread_t pt2;

    begin = clock();
    Create_File(TEN_FILE);
    end = clock();
    printf("Time run : %f\n", (float)(end-begin)/CLOCKS_PER_SEC);

    /* multithread */
    begin = clock();
    pthread_create(&pt1, NULL, Create_File, ZERO_TO_FIVE_FILE);
    pthread_create(&pt2, NULL, Create_File, FIVE_TO_TEN_FILE);
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);
    end = clock();
    printf("Time run : %f\n", (float)(end-begin)/CLOCKS_PER_SEC);
    return 0;
}

int randomly(int minN, int maxN)
{
	return minN + rand() % (maxN + 1 - minN);
} 

void WriteNumberToFile(int fd, int number)
{
    char *str = (char *)malloc(1 * sizeof(char));
    ssize_t write_fd;
    sprintf(str, "%d", number);
    /* Write content of number to newfile to new file */
    write_fd = write(fd, str, 1);
    free(str);
}

void Create_File(Types Type)
{
    int number = 0;
    int index = 0;
    int fd;
    clock_t begin;
    clock_t end;
    int length = 0;
    pthread_t pt1;
    pthread_t pt2;
    char *inputLink;
    int initValue, endValue = 0;

    switch (Type)
    {
    case TEN_FILE:
        initValue = 0;
        endValue = 10;
        inputLink = LINKNOMT;
        break;
    case ZERO_TO_FIVE_FILE:
        initValue = 0;
        endValue = 5;
        inputLink = LINKMT;
        break;
    case FIVE_TO_TEN_FILE:
        initValue = 5;
        endValue = 10;
        inputLink = LINKMT;
        break;
    default:
        /* Do nothing */
        break;
    }

    begin = clock();
    length = sizeof("/home/truong/workspace/Linux_Aphu/BBB_Linux/Multi_Threading/Test2/MThread/file0") / sizeof(char);
    srand((int) time(0));
    for(int i = initValue; i < endValue; i++)
    {
        char *str = (char *)malloc(length * sizeof(char));
        sprintf(str, inputLink, i);
        mode_t mode = S_IRUSR | S_IWUSR | S_IROTH| S_IWOTH;

        fd = creat((char *)str, mode);
        close(fd);
        fd = open(str, O_APPEND | O_WRONLY);
        free(str);
        if(fd != -1)
        {
            printf("open successful\n");
        }
        /* Get 5.000.000 numbers */
        for(index = 0; index < 5000000; index++)
        {
            /* Get number that has 1 digit */
            number = randomly(0,9);
            WriteNumberToFile(fd, number);
        }
        close(fd);
    }
}