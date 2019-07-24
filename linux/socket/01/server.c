#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> //这个必须要加上，不然会段错误。但是编译不会报错。这也是乱七八糟。

#define PORT 8888
#define BUF_SIZE 2048

int main()
{
	int listenfd, connfd, sockfd;
	struct sockaddr_in serveraddr = {}, clientaddr = {};
	int maxfd;
	int client[FD_SETSIZE] = {};
	int maxi, i;
	fd_set rset,allset;
	int nready;
	char buf[BUF_SIZE] = {};
	ssize_t n, ret;
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd < 0)
	{
		printf("socket create error \n");
		exit(1);
	}
	printf("listenfd:%d \n", listenfd);
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int optval = 1;
	//do it before bind
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
	{
		printf("setsockopt failed \n");
		exit(1);
	}
	
	if(bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
	{
		perror("bind error");
		exit(1);
	}
	if(listen(listenfd, 20) < 0)
	{
		perror("listen failed ");
		exit(1);
	}
	maxfd = listenfd;
	maxi = -1;
	for(i=0; i<FD_SETSIZE; i++)
	{
		client[i] = -1;
	}

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	while(1)
	{
		rset = allset;//add new fd to rset or delete the unused fd
		nready = select(maxfd+1, &rset, NULL, NULL, NULL);// select on connfd and listenfd
		if(FD_ISSET(listenfd, &rset))
		{
			int len = sizeof(clientaddr);
			printf("accept a connection \n");
			if((connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &len)) < 0)
			{
				perror("accept error \n");
				exit(1);
			}
			printf("accept client ip:%s, port:%d \n", inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port);
			for(i=0; i<FD_SETSIZE; i++)
			{
				if(client[i] < 0)
				{
					client[i] = connfd;
					break;
				}
			}
			if(i == FD_SETSIZE)
			{
				perror("too many connections \n");
				exit(1);
			}
			FD_SET(connfd, &allset);
			if(connfd > maxfd)
			{
				maxfd = connfd;

			}
			if(i > maxi)
			{
				maxi = i;
			}
			if(--nready < 0)
			{
				continue;
			}

		}
		for(i=0 ; i<=maxi; i++)
		{
			if((sockfd = client[i]) < 0)
			{
				continue;
			}
			if(FD_ISSET(sockfd, &rset))
			{
				printf("\nreading the socket \n");
				memset(buf, 0, BUF_SIZE);
				if((n= read(sockfd, buf, BUF_SIZE) ) <= 0)
				{
					close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				}
				else
				{
					printf("client[%d] send message:%s \n", i, buf);
					ret = write(sockfd, buf, n);
					if(ret != n)
					{
						printf("error write to socket \n");
						exit(1);
					}
				}
				if(--nready <= 0)
				{
					break;
				}
			}
		}
	}
}

