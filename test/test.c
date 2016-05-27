/*
*This program is used to test /dev/nvmd
* 1) open /dev/nvmd
* 2) mmap /dev/nvmd to the virtual address space of this process
* 3) read and write to the mapped address space.
* anyway, it works now.
*/


#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <string.h>
#include <malloc.h>

#define BUF_SIZE 4096

int main()
{
	int fd;
	char *buf =(char*) malloc(BUF_SIZE);
	void *start = NULL;
	fd = open("/dev/nvmd",O_RDWR);
	if(fd == -1)
	{
		printf("Failed to open /dev/nvmd ! \n");
		return 1;
	}
	
	start = mmap(NULL, 1UL<<32, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	printf("mmap successed ! start: %p !\n", start);

	memset(buf,0,BUF_SIZE);
	strcpy(buf, start);
	printf("buf::%s !\n", buf);	
	
	strcpy(start, "hello starking, this is a test program!\n");
	strcpy(buf, start);
	printf("buf2::%s !\n", buf);	
	

	char c= getchar();

	munmap(start, 1UL<<32);
	printf("munmap successed ! start: %p !\n", start);

	close(fd);
	return 0;
}
