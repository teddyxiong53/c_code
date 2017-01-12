#include <sys/select.h>
#include <stdio.h>
#include <fcntl.h>
#include <signal.h>

void input_handler(int signum)
{
	printf("recv a signal from globalfifo, num is %d \n", signum);
}
int main()
{
	int fd = -1;
	int oflags = 0;
	
	fd = open("/dev/globalfifo", O_RDWR, S_IRUSR | S_IWUSR);
	if(fd == -1)
	{
		printf("open dev failed \n");
		return -1;
	}
	printf("-----------fd is %d \n");
	signal(SIGIO, input_handler);
	fcntl(fd, F_SETOWN, getpid());
	oflags = fcntl(fd, F_GETFL);
	fcntl(fd, F_SETFL, oflags|FASYNC);
	while(1)
	{
		sleep(1);
	}
	
}