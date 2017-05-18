#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>


#define BUF_SIZE 2048 

ssize_t readline(int fd, char *vptr, size_t maxlen)
{
	ssize_t	n, rc;
	char	c, *ptr;

	ptr = vptr;
	for (n = 1; n < maxlen; n++) {
		if ( (rc = read(fd, &c,1)) == 1) {
			*ptr++ = c;
			if (c == '\n')
				break;	/* newline is stored, like fgets() */
		} else if (rc == 0) {
			*ptr = 0;
			return(n - 1);	/* EOF, n - 1 bytes were read */
		} else
			return(-1);		/* error, errno set by read() */
	}

	*ptr = 0;	/* null terminate like fgets() */
	return(n);
}
int main(int argc, char **argv)
{
	if(argc != 2)
	{
		printf("usage: %s ipaddr \n", argv[0]);
		exit(1);
	}
	struct sockaddr_in serveraddr;
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		printf("socket create error \n");
		exit(1);
	}
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(8888);
	if(inet_pton(AF_INET, argv[1], &serveraddr.sin_addr) < 0)
	{
		printf("inet_pton error \n");
		exit(1);
	}
	printf("ipaddr:%s, port:%d \n", inet_ntoa(serveraddr.sin_addr), serveraddr.sin_port);
	if(connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr) )< 0) 
	{
		printf("connect failed \n");
		exit(1);
	}
	char sendline[BUF_SIZE], recvline[BUF_SIZE];
	while(fgets(sendline, BUF_SIZE, stdin) != NULL)
	{
		write(sockfd, sendline, strlen(sendline));
		if(readline(sockfd, recvline, BUF_SIZE) == 0)
		{
			perror("server not exist \n");
			exit(1);
		}
		if(fputs(recvline, stdout) == EOF)
		{
			perror("fputs error \n");
			exit(1);
		}
		memset(sendline, 0, sizeof(BUF_SIZE));
	}
	return 0;
}
