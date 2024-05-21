#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 4096
#define name "Prg1"
#define message1 "Hello "
#define message2 "Truong!"

int main()
{
    /* Shared memory file descriptor */
    int shm_fd;
    /* Pointer to shared memory */
    void *ptr;
    /* Open the shared memory object */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);
    /* Set the size for shared memory */
    ftruncate(shm_fd, SIZE);
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
    sprintf(ptr, "%s", (char *)message1);
    ptr += strlen(message1);
    sprintf(ptr, "%s", (char *)message2);
    ptr += strlen(message2);

    return 0;
}