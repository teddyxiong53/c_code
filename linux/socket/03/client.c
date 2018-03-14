#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>


#include <arpa/inet.h>

#define BUFSIZE 64

int main(int arg, char **argv)
{
    if(argc < 2)
    {
        printf("usage: %s ipaddr port", basename(argv[0]));
        exit(-1);
    }
    char *ip = argv[1];
    int port = atoi(argv[2]);
    
    struct sockaddr_in serveraddr = {0};
    serveraddr.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &serveraddr.sin_addr);
    serveraddr.port = htons(port);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    assert(sockfd >= 0);
    int ret ;
    ret = connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    assert(ret == 0);
    
    pollfd fds[2];//注册stdin和sockfd的。
    fds[0].fd = 0;
    fds[0].events = POLLIN;
    fds[0].revents = 0;
    
    fds[1].fd = sockfd;
    fds[1].events = POLLIN | POLLRDHUP;
    fds[0].revents = 0;
    
    char readbuf[BUFSIZE];
    int pipefd[2];
    int ret = pipe(pipefd);
    assert(ret != -1);
    while(1)
    {
        ret = poll(fds, 2, -1);
        if(ret < 0)
        {
            printf("poll failed \n");
            break;
        }
        
        if(fds[1].revents & POLLRDHUP)
        {
            printf("server close the conn \n");
            break;
        }
        else if(fds[1].revents & POLLIN)
        {
            memset(readbuf, 0, BUFSIZE);
            recv(sockfd, readbuf, BUFSIZE-1, 0);
            printf("%s \n", readbuf);
            
        }
        if(fds[0].revents &POLLIN)
        {
            ret = splice(0 /*stdin*/, NULL, pipefd[1], NULL, 32768,  SPLICE_F_MOVE|SPLICE_F_MORE);
            ret = splice(pipefd[0], NULL, sockfd, NULL,  32768,  SPLICE_F_MOVE|SPLICE_F_MORE);
        }
    }
    close(sockfd);
    return 0;
}