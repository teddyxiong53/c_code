#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


int main()
{
	int fd ;
	fd = open("/dev/mymisc", O_RDONLY);
	if(fd < 0)
	{
		//printf("open mymisc failed \n");
		perror("open mymisc failed ");
		exit(1);
	}
	if(ioctl(fd, 6) != 0)
	{
		
	}
	close(fd);
	return 0;
}