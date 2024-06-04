#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <memory.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/mman.h>

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

    /* Create a memory in RAM */
    ptr = mmap(
        NULL, /* The starting address for the new mapping can be specified */
        size*sizeof(int), /* argument specifies the length of the mapping (which must be greater than 0) */
        PROT_READ | PROT_WRITE, /*  argument describes the desired memory protection of the mapping.*/
        MAP_PRIVATE | MAP_ANONYMOUS,
        0,
        0
    );

    if(ptr == MAP_FAILED){
        printf("mmap failed: %s\n", strerror(errno));
        return 1;
    }

    for(idx = 0; idx < size; idx++){
        ptr[idx] = idx;
    }

    for(idx = 0; idx < size; idx++){
        printf("ptr[%d] = %d\n", idx, ptr[idx]);
    }

    err = munmap(ptr, size*sizeof(int));
    if(err != 0){
        printf("munmap failed: %s\n", strerror(errno));
        return 1;
    }
    
    return 0;
}