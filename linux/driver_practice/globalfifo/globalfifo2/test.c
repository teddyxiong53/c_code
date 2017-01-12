#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>

int main()
{
	int fd = -1;
	fd_set rfds, wfds;
	
	fd = open("/dev/globalfifo", O_RDONLY| O_NONBLOCK);
	if(fd == -1)
	{
		printf("open dev failed \n");
		return -1;
	}
	printf("-----------fd is %d \n");
	
	if(ioctl(fd, 0x1, 0) <0)
	{
		printf("clear buffer failed \n");
	}
	while(1)
	{
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(fd, &rfds);
		FD_SET(fd, &wfds);
		select(fd+1, &rfds, &wfds, NULL, NULL);
		if(FD_ISSET(fd, &rfds))
		{
			printf("poll can be read \n");
		}
		if(FD_ISSET(fd, &wfds))
		{
			printf("poll can be writen \n");
		}
		
	}
	
}