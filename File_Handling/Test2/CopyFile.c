#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>

#define linkconf_oldfile "/home/truong/workspace/Linux_Aphu/BBB_Linux/File_Handling/Test2/OldFile.txt"
#define linkconf_newfile "/home/truong/workspace/Linux_Aphu/BBB_Linux/File_Handling/Test2/NewFile.txt"

bool clearFileContent(int fd);
bool printContentOfFile(int fd);

int main()
{
    int position_read = 0;
    char *buffer = NULL;
    int open_old, open_new = 0;
    off_t return_lseek = 0;
    struct stat st;    
    int size = 0;
    ssize_t read_fd = 0; 
    ssize_t write_fd = 0;

    open_old = open(linkconf_oldfile, O_APPEND | O_RDONLY);
    if (open_old == -1)
    {
        /* print which type of error have in a code */
        printf("Old file:  Error Number % d\n", errno);
        /* print program detail "Success or failure" */
        perror("Program");
    }
    else
    {
        if (((clearFileContent(open_new)) == false) || (printContentOfFile(open_new) == false)) 
        {
            /* print which type of error have in a code */
            printf("New file:  Error Number % d\n", errno);
            /* print program detail "Success or failure" */
            perror("Program");
        }
        else
        {
            open_new = open(linkconf_newfile, O_WRONLY);
            printf("Open new file sucecssfully\n");
            stat(linkconf_oldfile, &st);
            /* Get length of old file */
            size = st.st_size;

            /* Allocate memory with the size equal to file */
            buffer = (char *) malloc(size * sizeof(char *));
            /* Set cursor to start of file */
            return_lseek = lseek(open_old, 0, SEEK_SET);
            /* Read the whole content of old file and save to buffer */
            read_fd = read(open_old, buffer, size);
            printf("Content of old file is :\n%s\n", buffer);
            /* Write content of saved buffer to new file */
            write_fd = write(open_new, buffer, size);
            /* Read new file */
            read_fd = read(open_new, buffer, size);
            printf("Content of new file is :\n%s\n", buffer);
            free(buffer);
            close(open_new);
            close(open_old);
        }
    }
    
    return 0;
}

bool clearFileContent(int fd)
{
    int ret = 0;
    ssize_t write_fd = 0;
    struct stat st;
    int size = 0;

    /* O_WRONLY will erase all content of file when open file */
    fd = open(linkconf_newfile, O_WRONLY);
    if (fd == -1)
    {
        return false;
    }
    else
    {
        close(fd);
        return true;
    }

}

bool printContentOfFile(int fd)
{
    int ret = 0;
    ssize_t read_fd = 0;
    char *buffer = NULL;
    struct stat st;
    int size = 0;
    int return_lseek = 0;

    fd = open(linkconf_newfile, O_RDONLY);
    if (fd == -1)
    {
        return false;
    }
    else
    {
        stat(linkconf_newfile, &st);
        /* Get length of old file */
        size = st.st_size;
        return_lseek = lseek(fd, 0, SEEK_SET);
        /* Allocate memory with the size equal to file */
        buffer = (char *) malloc(size * sizeof(char *));
        /* Read the whole content of old file and save to buffer */
        read_fd = read(fd, buffer, size);
        printf("Content of new file before being copied is :\n%s\n", buffer);
        close(fd);
        free(buffer);
        return true;
    }
}
