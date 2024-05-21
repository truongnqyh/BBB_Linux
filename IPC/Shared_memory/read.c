#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdlib.h>

#define SIZE 4096
#define name "Prg1"

int main()
{
    /* Shared memory file descriptor */
    int shm_fd;
    /* Pointer to shared memory */
    void *ptr;
    /* Open the shared memory object */
    shm_fd = shm_open(name, O_RDWR, 0666);
    /* Memory map the shared memory object */
    /* Desired memory protection of the mapped region:
    * PROT_READ: Pages may be read.
    * PROT_WRITE: Pages may be written.
    * PROT_EXEC: Pages may be executed.
    * PROT_NONE: Pages cannot be accessed.
    */
   /* Memory map the shared memory object
   * If address_ptr = NULL -> automatically allocate suitable address for the object
   */
    ptr = mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    /* Read from the shared memory object */
    printf("%s\n", (char *)ptr);

    /* Remove the shared memory object */
    shm_unlink(name);

    return 0;
}