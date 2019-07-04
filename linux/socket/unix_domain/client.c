#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include "mylog.h"

char *filename = "unix_socket_1";
#define BUFFER_SIZE 1024


int main()
{
    struct sockaddr_un un;
    char buffer[BUFFER_SIZE] = {0};
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, filename);
    int sock_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(sock_fd < 0) {
        myloge("socket fail");
        return -1;
    }
    int ret;
    ret = connect(sock_fd, (struct sockaddr*)&un, sizeof(un));
    if(ret < 0) {
        myloge("connect fail");
        close(sock_fd);
        return -1;
    }
    strcpy(buffer, "hello unix domain socket");
    send(sock_fd, buffer, strlen(buffer), 0);
    close(sock_fd);
    return 0;
}
