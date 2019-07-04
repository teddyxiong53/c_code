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
    int ret;
    int len;
    int new_fd;
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(fd < 0) {
        myloge("socket fail");
        return -1;
    }
    un.sun_family = AF_UNIX;
    unlink(filename);
    strcpy(un.sun_path, filename);

    ret = bind(fd, (struct sockaddr*)&un, sizeof(un)) ;
    if(ret < 0) {
        myloge("bind fail");
        goto err1;
    }
    ret = listen(fd, 10);
    if(ret < 0) {
        myloge("listen fail");
        goto err1;
    }
    while(1) {
        struct sockaddr_un client_addr;
        char buffer[BUFFER_SIZE];
        memset(buffer, 0, BUFFER_SIZE);
        len = sizeof(client_addr);
        new_fd = accept(fd, NULL, NULL);
        if(new_fd < 0) {
            myloge("accept fail");
            goto err1;
        }
        ret = recv(new_fd, buffer, BUFFER_SIZE, 0);
        if(ret < 0) {
            myloge("recv fail");
            goto err1;
        }
        mylogd("recv content:[%s]", buffer);
        break;
    }
    close(fd);
    return 0;
err1:
    close(fd);
    return -1;
}