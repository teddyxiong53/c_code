#include "rtsp.h"


struct rtsp_buffer *rtsp[MAX_CONN];

int rtsp_get_free_num(void)
{
    int i;
    for(i=0; i<MAX_CONN; i++) {
        if(rtsp[i]->conn_status == 0) {
            return i;
        }
    }
    return -1;
}


int rtsp_init_buffer(void)
{
    int i;
    for(i=0; i<MAX_CONN; i++) {
        rtsp[i] = malloc(sizeof(struct rtsp_buffer));
        if(rtsp[i] == NULL) {
            myloge("malloc failed");
            return -1;
        }
        memset(rtsp[i], 0, sizeof(struct rtsp_buffer));
        
    }
	return 0;
}


int rtsp_get_method(int num)
{
    struct rtsp_buffer *rb;
    rb = rtsp[num];
    if(rb->in_buffer[0] == '\0') {
        return -1;
    }
    char method[32];
    sscanf(rb->in_buffer, "%31s", method) ;
    int ret;
    if(strcmp(method, "OPTIONS") == 0) {
        return 1;
    }
    if(strcmp(method, "DESCRIBE") == 0) {
        return 2;
    }
    if(strcmp(method, "SETUP") == 0) {
        return 3;
    }

    if(strcmp(method, "PLAY") == 0) {
        return 4;
    }
    if(strcmp(method, "TEARDOWN") == 0) {
        return 5;
    }
    return -1;
}


static char *rtsp_get_stat(int err)
{
    struct {
        char *token;
        int code;
    } status [] = {
        "Continue", 100,
        "OK", 200,
        "Created", 201,
        "Accepted", 202,
        "Non-Authoritative Infomation", 203,
        "No Content", 204,
        "Reset Content", 205,
        "Partial Content", 206,
        "Multiple Choices", 300,
        "Moved Permanently", 301,
        "Moved Temporarily", 302,
        "Bad Request", 400,
        "Unauthorized", 401,
        "Payment Required", 402,
        "Forbidden", 403,
        "Not Found", 404,
        "Method Not Allowed", 405,
        "Not Acceptable", 406,
        "Proxy Authentication Required", 407,
        "Request Time-out", 408,
        "Conflict", 409,
        "Gone", 410,
        "Length Required", 411,
        "Precondition Failed", 412,
        "Request Entity Too Large", 413,
        "Request-URI Too Large", 414,
        "Unsupported Media Type", 415,
        "Bad Extension", 420,
        "Invalid Parameter", 450,
        "Parameter Not Understood", 451,
        "Conference Not Found", 452,
        "Not Enough Bandwidth", 453,
        "Session Not Found",454,
        "Method Not Valid In The State", 455,
        "Header Field Not Valid for Resource", 456,
        "Invalid Range", 457,
        "Parameter Is Read-Only", 458,
        "Unsupported transport", 461,
        "Internal Server Error", 500,
        "Not Implemented", 501,
        "Bad Gateway", 502,
        "Service Unavailable", 503,
        "Gateway Time-out", 504,
        "RTSP Version Not Supported", 505,
        "Option not supported", 551,
        "Extended Error:", 911,
        NULL, -1
    };
    int i;
    for(i=0; status[i].code != err && status[i].code != -1; i++) {
        ;
    }
    return status[i].token;
}

static int rtsp_send_reply(int err, int conn_num)
{
    char err_buf[1024] = {0};
    sprintf(err_buf, "%s %d %s" RTSP_EL"CSeq: %d"RTSP_EL, 
        RTSP_VER, err,
        rtsp_get_stat(err),
        rtsp[conn_num]->rtsp_cseq
        );
    strcat(err_buf, RTSP_EL);
    int ret;
    ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, err_buf, strlen(err_buf));
    if(ret < 0) {
        myloge("write failed");
        return -1;
    }
    return 0;
}


static int rtsp_get_cseq(int conn_num)
{
    char *p;
    char trash[255] = {0};
    p = strstr(rtsp[conn_num]->in_buffer, "CSeq" );
    if(p == NULL) {
        rtsp_send_reply(400, conn_num);
    } else {
        int ret;
        ret = sscanf(p, "%254s %d", trash, &(rtsp[conn_num]->rtsp_cseq));
        if(ret != 2) {
            rtsp_send_reply(400, conn_num);
            return -1;
        }
    }
    return 0;
}

static int rtsp_send_reply_options(int status, int conn_num)
{
    sprintf(rtsp[conn_num]->out_buffer, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL, 
        RTSP_VER, status, rtsp_get_stat(status), rtsp[conn_num]->rtsp_cseq);
    strcat(rtsp[conn_num]->out_buffer, "Public: OPTIONS,DESCRIPTION,SETUP,PLAY,PAUSE,TEARDOWN"RTSP_EL);
    strcat(rtsp[conn_num]->out_buffer, RTSP_EL);
    int ret;
    ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, rtsp[conn_num]->out_buffer, strlen(rtsp[conn_num]->out_buffer));
    if(ret) {
        return -1;
    }
    return 0;
}



static int rtsp_method_options(int conn_num)
{
    int ret;
    ret = rtsp_get_cseq(conn_num);
    if(ret == 0) {
        rtsp_send_reply_options(200, conn_num);
        return 0;
    }
    return -1;
}


int rtsp_proc_method(int method, int conn_num)
{
    int ret;
    switch(method) {
        case 1:
            ret = rtsp_method_options(conn_num);
            if(ret < 0) {
                mylogd("options fail");
                close(rtsp[conn_num]->cli_rtsp.cli_fd);
                rtsp[conn_num]->rtspd_status = 0x01;
                rtsp[conn_num]->is_running = 0;
            } else {
                mylogd("options ok");
                rtsp[conn_num]->rtspd_status = 0x02;
            }
            break;
    }
    return 0;

}
void *rtsp_proc(void *arg)
{
    int conn_num = (int)arg;
    pthread_detach(pthread_self());
    int ret;
    int fd = rtsp[conn_num]->cli_rtsp.cli_fd;
    while(1) {
        ret = socket_tcp_read(fd, rtsp[conn_num]->in_buffer, 4096);
        if(ret < 0) {
            break;
        }
        int method;
        method = rtsp_get_method(conn_num);
        ret = rtsp_proc_method(method, conn_num);
        if(ret < 0) {

        }
    }

    pthread_exit(NULL);
}