#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#define MAGIC_NO 100
#define SEND_DATA_CMD _IOW(MAGIC_NO, sizeof(struct config), struct config *)
#define GET_DATA_CMD _IOR(MAGIC_NO, sizeof(struct config), struct config *)

struct config
{
    int time;
};

int main()
{
    int fd = 0;
    char data[8];
    struct config config_data;

    config_data.time = 10;
    fd = open("/dev/misc_example", O_RDWR);
    if (fd > 0)
    {
        memset(data, 0, sizeof(data));

        ioctl(fd, SEND_DATA_CMD, &config_data);
        write(fd, "1", strlen("1"));

        read(fd, data, 1);
        write(fd, "0", strlen("0"));
        close(fd);
    }

    return 0;
}
