#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdbool.h>
#include <time.h>

#define linkconf_file "/home/truong/workspace/Linux_Aphu/BBB_Linux/File_Handling/Test3/test.txt"
#define filename "test.txt"

void getModeFile(struct stat status);

int main()
{
    int position_read = 0;
    char *buffer = NULL;
    int fd = 0;
    off_t return_lseek = 0;
    struct stat st;    

    fd = open(linkconf_file, O_APPEND | O_RDONLY);
    if (fd == -1)
    {
        /* print which type of error have in a code */
        printf("Old file:  Error Number % d\n", errno);
        /* print program detail "Success or failure" */
        perror("Program");
    }
    else
    {
        printf("------------Open new file sucecssfully------------\n");
        /* Get the information of file */
        stat(linkconf_file, &st);
        /* Show the information of file */
        printf("-ID of device containing file     : %ld\n", st.st_dev);
        printf("-Inode number                     : %ld\n", st.st_ino);
        printf("-File type and mode               : \n");
        getModeFile(st);
        printf("-Number of hard links             : %ld\n", st.st_nlink);
        printf("-User ID of owner                 : %d\n", st.st_uid);
        printf("-Group ID of owner                : %d\n", st.st_gid);
        printf("-Device ID (if special file)      : %ld\n", st.st_rdev);
        printf("-Total size, in bytes             : %ld\n", st.st_size);
        printf("-Block size for file system I/O   : %ld\n", st.st_blksize);
        printf("-Number of 512B blocks allocated  : %ld\n", st.st_blocks);
        printf("Time of last access               : %s\n", ctime(&st.st_atime));
        printf("Time of last modification         : %s\n", ctime(&st.st_mtime));
        printf("Time of last status change        : %s\n", ctime(&st.st_ctime));
        close(fd);
    }
    return 0;
}

void getModeFile(struct stat status)
{
 // Get information about the file
    if (stat(filename, &status) == 0)
    {
        mode_t file_mode = status.st_mode;

        // Check if it's a regular file
        if (S_ISREG(file_mode)) 
        {
            printf("    + %s is a regular file.\n", filename);
        }

        // Check if it's a directory
        if (S_ISDIR(file_mode)) 
        {
            printf("    + %s is a directory.\n", filename);
        }

        // Check permissions
        if (file_mode & S_IRUSR) 
        {
            printf("    + Owner has read permission.\n");
        }
        if (file_mode & S_IWUSR) 
        {
            printf("    + Owner has write permission.\n");
        }
        if (file_mode & S_IXUSR) 
        {
            printf("    + Owner has execute permission.\n");
        }
    } 
    else 
    {
        perror("    + stat() failed");
    }
}