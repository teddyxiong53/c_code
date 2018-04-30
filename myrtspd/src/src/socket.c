#include "rtsp.h"
#include <pthread.h>


static int socket_really_proc_accept(void)
{
    int cli_len;
    int conn_num;
    conn_num = rtsp_get_free_num();
    if(conn_num < 0) {
        myloge("no free conn buffer");
        return -1;
    }
    struct rtsp_buffer *rb;
    rb = rtsp[conn_num];
    rb->cli_rtsp.conn_num = conn_num;

    struct sockaddr_in cli_addr;
    cli_len = sizeof(cli_addr);
    rb->cli_rtsp.cli_fd = accept(rtsp[0]->fd.rtspfd, (struct sockaddr *)&cli_addr, &cli_len);
    if(rb->cli_rtsp.cli_fd < 0) {
        perror("accept fail");
        return -1;
    }
    return 0;
}


static void *socket_proc_accept(void *arg)
{
    pthread_detach(pthread_self());
    while(1) {
        socket_really_proc_accept();
    }
    pthread_exit(NULL);
}

int socket_tcp_read(int fd, void *buf, int len)
{
    int read_bytes;
    char *p = (char *)buf;
    int bytes_left = len;
    while(1) {
        read_bytes = read(fd, p, bytes_left);
        if(read_bytes < 0) {
            if(errno == EINTR) {
                continue;
            } else {
                return -1;
            }

        } else {
            break;
        }
    }
    return read_bytes;
}

int socket_create_server(char *host, u16 port)
{
    int ret;
    int listenfd = 0;
    rtsp[0]->fd.rtspfd = socket(AF_INET, SOCK_STREAM ,0);
    if(rtsp[0]->fd.rtspfd < 0) {
        perror("socket fail");
        //return -1;
        goto error1;
    }
    listenfd = rtsp[0]->fd.rtspfd;

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(host);
    server_addr.sin_port = htons(port);
    int one = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));

    ret = bind(rtsp[0]->fd.rtspfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret) {
        perror("bind fail");
        //return -1;
        goto error2;
    }
    ret = listen(rtsp[0]->fd.rtspfd, 5);
    if(ret) {
        perror("listen fail");
        goto error2;
    }
    mylogd("listen ok");
    ret = pthread_create(&rtsp[0]->pth.rtsp_vthread1, NULL, socket_proc_accept, NULL);
    if(ret ) {
        perror("pthread create fail");
        goto error2;
    }
    return 0;
error2:
    close(rtsp[0]->fd.rtspfd);
error1:
    return -1;
}