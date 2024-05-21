#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

char buffer[2048];
#define NUM_READ 10

int main(int argc, char *argv[])
{
	int fd;
	int remaining = NUM_READ;
	int total_read = 0;
	int n = 0;
	int ret = 0;

	if(argc != 2 ){
		printf("Wrong usage\n");
		printf("Correct usage: <file> <readcount>\n");
		return 0;
	}
	/* Convert command line supplied data to integer */
	remaining = atoi(argv[1]);
	printf("read requested = %d\n", remaining);
	/* Open the device file */
	fd = open("/dev/pcdev-0",O_RDONLY);
	if(fd < 0){
		/* perror decodes user space errno variable and prints cause of failure */
		perror("open");
		return fd;
	}
	else{
		printf("open was successful\n");
	}
	/*Lets attempt reading twice */
	while((n != 2) && remaining)
	{
		/* Read data with number of bytes remaining */
		ret = read(fd, &buffer[total_read], remaining);
		if(!ret){
			/* End of file */
			printf("end of file \n");
			break;
		}else if(ret <= remaining){
			printf("read %d bytes of data \n",ret );
			/* Calculate the total number of bytes read */
		    total_read += ret;
			/* Update the remaining number of bytes to read */
			remaining -= ret;
		}else if(ret < 0){
			printf("something went wrong\n");
			break;
		}else
			break;
		n++;
	}
	/* Print out the total number of bytes read */
	printf("total_read = %d\n",total_read);		
	/* Print out the buffer */
	for(int i=0 ; i < total_read ; i++){
		printf("%c",buffer[i]);
	}
	close(fd);
	
	return 0;
}