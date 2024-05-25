#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>
#include <fcntl.h>

#define FILE_PATH "test.txt"
typedef struct message{
    char *name;
    int age;
    int class;
}message_t;
/*** void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset)
 *
 * addr: The starting address for the new mapping can be specified
 * length: The starting address for the new mapping can be specified
 * prot: argument describes the desired memory protection of the mapping. 
    * PROT_EXEC: Pages may be executed.
    * PROT_READ: Pages may be read.
    * PROT_WRITE: Pages may be written.
    * PROT_NONE: Pages may not be accessed.
 * flags: argument specifies the type of mapping to be created.
    * MAP_SHARED: The mapping is shared.
    * MAP_PRIVATE: The mapping is private.
    * MAP_ANONYMOUS: The mapping is not backed by any file.
 * fd: file descriptor
 * offset: specifies the starting position in the file where the mapping begins
***/
int main(int argc, char *argv[]){
    int *ptr;
    int size = 5;
    int idx;
    int err; 
    int fd;
    message_t msg;
    struct stat stat_buf;
    char name;

    name = "BeKem";
    msg.age = 12;
    msg.class = 2;
    msg.name = &name;

    /* open file */
    fd = open(FILE_PATH, O_RDWR, 0666);
    if(fd == -1){
        printf("open failed: %s\n", strerror(errno));
        return 1;
    }
    /* get information of file */
    err = fstat(fd, &stat_buf);
    if(err == -1){
        printf("fstat failed: %s\n", strerror(errno));
        return 1;
    }
    /* Create a memory in RAM */
    ptr = mmap(
        NULL,
        stat_buf.st_size + sizeof(message_t),
        PROT_READ | PROT_WRITE,
        MAP_SHARED,
        fd,
        0
    );
    if(ptr == MAP_FAILED){
        printf("mmap failed: %s\n", strerror(errno));
        return 1;
    }
    close(fd);
    /* copy data to shared RAM */
    memcpy(ptr, &msg, sizeof(message_t));
    /* sync data from RAM to file(external storage) */
    msync(ptr, sizeof(message_t), MS_SYNC);
    err = munmap(ptr, size*sizeof(int));
    if(err != 0){
        printf("munmap failed: %s\n", strerror(errno));
        return 1;
    }
    
    return 0;
}