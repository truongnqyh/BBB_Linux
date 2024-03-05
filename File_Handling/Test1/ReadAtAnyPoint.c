#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>

#define linkconf "/home/truong/workspace/Linux_Aphu/BBB_Linux/File_Handling/Test1/Test.txt"
extern int errno;

int main()
{
    int position_read = 0;
    char *buffer = NULL;
	int open_fd,read_fd = 0; 
    off_t return_lseek = 0;
    struct stat st;
    int size = 0;

    open_fd = open(linkconf, O_APPEND | O_RDONLY);
    if (open_fd == -1)
    {
        // print which type of error have in a code
        printf("Error Number % d\n", errno);
        // print program detail "Success or failure"
        perror("Program");
    }
    else
    {
        printf("Open file sucecssfully\n");
        stat(linkconf, &st);
        size = st.st_size;
        printf("length of file is: %d\n", size);
    do
    {
        printf("Enter the position to read: ");
        scanf("%d", &position_read);
        if ((position_read >= 0) && (position_read <= size))
        {
            break;
        }
        else
        {
            printf("You must choose position from 0 to %d\n", size); 
        }
    }while(1);
        buffer = (char *) malloc(size * sizeof(char *));
        return_lseek = lseek(open_fd, position_read, SEEK_SET);
        read_fd = read(open_fd, buffer, (size - position_read));
        printf("Content of file from position %d is :\n%s\n",position_read, buffer);
        free(buffer);
        close(open_fd);
    }
    
    return 0;
}