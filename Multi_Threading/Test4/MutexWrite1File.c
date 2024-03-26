#include <stdio.h>
#include <pthread.h>
#include <Write.h>

/* This's main thread func */

/* Using only one variable sum will raise error, race condition -> need to use 2 seperated variable or use lock (mutex, smp, ...)
int sum = 0; -> wrong 
volatile sum = 0; -> wrong
*/

int main()
{
    int sum = 0;
    int i = 0;
    int *fd = 0;
    pthread_t pt1;
    pthread_t pt2;
    ssize_t read_fd = 0;
    char *buffer = NULL;
    off_t return_lseek = 0;
    struct stat st;   
    int size = 0;

    fd = open(linkconf, O_WRONLY | O_APPEND);
    init_sum_lib(fd);
    pthread_create(&pt1, NULL, Hello, fd);
    pthread_create(&pt2, NULL, World, fd);
    pthread_join(pt1, NULL);
    pthread_join(pt2, NULL);
    free_sum_lib();

    stat(linkconf, &st);
    /* Get length of old file */
    size = st.st_size;
    /* Allocate memory with the size equal to file */
    buffer = (char *) malloc(size * sizeof(char *));
    read_fd = read(*fd, buffer, size);
    printf("Content of file is :\n%s\n", buffer);
    free(buffer);
    close(*fd);

    return 0;
}