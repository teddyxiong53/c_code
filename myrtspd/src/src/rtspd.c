#include "mylog.h"
#include "rtsp.h"
#include "global.h"
#include "md5.h"
#include "rtcp.h"

sem_t rtspd_lock[MAX_CONN];

int rtspd_basic_init(void)
{
    int ret;
    ret = rtsp_init_buffer();
    if(ret) {
        mylogd("rtsp_init_buffer fail");
        return ret;
    }
    int i;
    for(i=0; i<MAX_CONN; i++) {
        sem_init(&rtspd_lock, 0, 1);
    }
    
    
    return 0;
}

int rtspd_port_init(char *ip, u16 port)
{
    strcpy(rtsp[0]->hostname, ip);
    rtsp[0]->rtsp_deport = port;
    int ret = socket_create_server(rtsp[0]->hostname, rtsp[0]->rtsp_deport);
    if(ret) {
        myloge("socket listen create fail");

    }
    return ret;
}

static void rtspd_set_framerate(int fr, int conn_num)
{
    switch(fr) {
        case 25:
            rtsp[conn_num]->cmd_port.frame_rate_step = 3600;
            break;
    }

}

int rtspd_rtp_create_socket(char *host, u16 port, int conn_num)
{
    int ret;
    rtsp[conn_num]->fd.video_rtp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(rtsp[conn_num]->fd.video_rtp_fd < 0) {
        return -1;
    }
    int one = 1;
    setsockopt(rtsp[conn_num]->fd.video_rtp_fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    struct sockaddr_in rtp_addr, rtp_bind;
    rtp_bind.sin_family = AF_INET;
    rtp_bind.sin_addr.s_addr = htonl(INADDR_ANY);
    rtp_bind.sin_port = htons(rtsp[conn_num]->cmd_port.rtp_ser_port);
    ret = bind(rtsp[conn_num]->fd.video_rtp_fd, (struct sockaddr *)&rtp_bind, sizeof(rtp_bind) );
    if(ret < 0) {
        myloge("bind fail");
        goto error1;
    }
    rtp_addr.sin_family = AF_INET;
    rtp_addr.sin_addr.s_addr = inet_addr(host);
    rtp_addr.sin_port = htons(port);
    int len;
    len = sizeof(struct sockaddr_in);
    ret = connect(rtsp[conn_num]->fd.video_rtp_fd, (struct sockaddr *)&rtp_addr, len);
    if(ret < 0) {
        perror("connect fail");
        goto error1;
    }

    return 0;
error1:
    close(rtsp[conn_num]->fd.video_rtp_fd);
    return -1;
}

int rtspd_status(int conn_num)
{
	return rtsp[conn_num]->rtspd_status;
}

static rtspd_rtp_init(int conn_num)
{
    int ret;
    ret = rtspd_status(conn_num);
    if(ret != 8) {
        myloge("status is not right. status:%d", ret);
        return -1;
    }
    ret = rtspd_rtp_create_socket(rtsp[conn_num]->cli_rtsp.cli_host, 
        rtsp[conn_num]->cmd_port.rtp_cli_port,
        conn_num);
    if(ret < 0) {
        myloge("rtspd_rtp_create_socket fail");
        rtsp[conn_num]->rtspd_status = 0x14;
        return -1;
    }
    rtsp[conn_num]->rtspd_status = 0x14;
    return 0;
}

static int rtspd_rtcp_create_socket(char *host, u16 port, int conn_num)
{
    int ret;
    struct sockaddr_in rtcp_bind ,rtcp_addr;
    int one = 1;
    rtsp[conn_num]->fd.video_rtcp_fd = socket(AF_INET, SOCK_DGRAM, 0);
    int fd = rtsp[conn_num]->fd.video_rtcp_fd;
	mylogd("fd:%d",fd);
    if(fd < 0) {
        myloge("socket fail");
        goto error1;
    }
    setsockopt(fd, SOL_SOCKET ,SO_REUSEADDR, &one, sizeof(one));
    rtcp_bind.sin_family = AF_INET;
    rtcp_bind.sin_addr.s_addr = htonl(INADDR_ANY);
    rtcp_bind.sin_port = htons(rtsp[conn_num]->cmd_port.rtcp_ser_port);
    ret = bind(fd, (struct sockaddr *)&rtcp_bind, sizeof(rtcp_bind));
    if(ret ) {
        perror("bind fail");
        goto error2;
    }
    rtcp_addr.sin_family = AF_INET;
    rtcp_addr.sin_addr.s_addr = inet_addr(host);
    rtcp_addr.sin_port = htons(port);
    int len;
    len = sizeof(rtcp_addr);
    ret = connect(fd, (struct sockaddr *)&rtcp_addr, len);
    if(ret) {
        perror("connect fail");
        goto error2;
    }

    return 0;
error2:
    close(fd);
error1:
    return -1;
}

int md32(char *string, int length)
{
	MD5_CTX context;
  	union {
	  	CHAR c[16];
	  	U32 x[4];
  	} digest;
  	U32 r;
  	S32 i;
  
	MD5Init(&context);
	MD5Update(&context, string, length);
	MD5Final((U8 *)&digest, &context);
	r=0;
	for (i=0; i<3; i++) {
		r ^= digest.x[i];
	}

	return r;
}

int random32(int  type)
{
    struct {
        int type;
        struct timeval tv;
        clock_t cpu;
        pid_t pid;
        int hid;
        uid_t uid;
        gid_t gid;
        struct utsname name;
    } s;
    gettimeofday(&s.tv, NULL);
    uname(&s.name);
    s.type = type;
    s.cpu = clock();
    s.pid = getpid();
    s.hid = gethostid();
    s.uid = getuid();
    s.gid = getgid();

    return md32((char *)&s, sizeof(s));

}
int rtspd_rtcp_packet(struct rtcp_pkt *pkt)
{
    struct rtcp_header hdr;
    struct rtcp_header_sdes hdr_sdes;

    pkt->comm.version = 2;
    pkt->comm.padding = 0;
    pkt->comm.count = 1;
    pkt->comm.pt = SDES;
    
    int hdr_size = sizeof(hdr);
    int hdr_sdes_size = sizeof(hdr_sdes);
    int name_size = strlen(rtsp[0]->hostname);
    int pkt_size = sizeof(struct rtcp_pkt) + name_size;

    pkt->comm.length = htons((pkt_size>>2) - 1);
    int local_ssrc = random32(0);
    pkt->sdes.ssrc = htonl(local_ssrc);
    pkt->sdes.attr_name = CNAME;
    pkt->sdes.len = name_size;
    memcpy(pkt->sdes.name, rtsp[0]->hostname, name_size);
    return pkt_size;

}



void *rtspd_rtcp_proc(void *arg)
{
    pthread_detach(pthread_self());
    int conn_num = (int)arg;
    int len ;
    struct rtcp_pkt pkt;
    len = rtspd_rtcp_packet(&pkt);
    int ret;
    while(rtsp[conn_num]->is_running) {
        ret = write(rtsp[conn_num]->fd.video_rtcp_fd, &pkt, len ) ;
        if(ret < 0) {
            myloge("write fail");
            rtsp[conn_num]->rtspd_status = 0x22;
        }
        sleep(3);
    }
    close(rtsp[0]->fd.video_rtcp_fd);
    pthread_exit(NULL);

}


static int rtspd_rtcp_init(int conn_num)
{
    int ret;
    ret = rtspd_rtcp_create_socket(rtsp[conn_num]->cli_rtsp.cli_host, 
        rtsp[conn_num]->cmd_port.rtcp_cli_port, conn_num);
    if(ret < 0) {
        rtsp[conn_num]->rtspd_status = 0x17;
        return -1;
    }
    rtsp[conn_num]->rtspd_status = 0x18;
    ret = pthread_create(&rtsp[conn_num]->pth.rtcp_vthread, NULL, rtspd_rtcp_proc, (void *)conn_num);
    if(ret) {
        myloge("pthread create fail");
        return -1;
    }
    return 0;
}

int rtspd_vtype(int conn_num)
{
    return rtsp[conn_num]->vist_type;
}

void rtspd_rtp_free(int conn_num)
{
    close(rtsp[conn_num]->fd.video_rtp_fd);
}

void rtspd_rtcp_free(int conn_num)
{
    close(rtsp[conn_num]->fd.video_rtcp_fd);
    rtsp[conn_num]->is_running = 0;
}

void rtspd_rtsp_free(void)
{
    close(rtsp[0]->fd.rtspfd);

}

void rtspd_free(void)
{
    int i;
    for(i=0 ; i<MAX_CONN; i++) {
        if(rtsp[i] != NULL) {
            free(rtsp[i]);
            rtsp[i] = NULL;
        }
    }

}

void *rtspd_rtp_send_file(void *arg)
{
    int conn_num = (int)arg;
    pthread_detach(pthread_self());
    rtp_send_packet(conn_num);
    pthread_exit(NULL);
    
}
extern void *rtsp_proc(void *arg);
int main(int argc, char **argv)
{
    int ret;
	ret = rtspd_basic_init();
    if(ret) {
        return -1;
    }
    ret = rtspd_port_init("192.168.190.137", 554);
    if(ret) {
        return -1;
    }
    int conn_num = rtsp_get_free_num();
    mylogd("conn_num:%d", conn_num);

    rtspd_set_framerate(25, conn_num);
    ret = pthread_create(&rtsp[conn_num]->pth.rtsp_vthread, NULL, rtsp_proc, (void *)conn_num);
    if(ret) {
        myloge("pthread create fail");
        return -1;
    }
	ret = rtspd_rtp_init(conn_num);
	if(ret) {
        myloge("rtspd_rtp_init fail");
        return -1;
    }
    ret = rtspd_rtcp_init(conn_num);
    if(ret) {
        myloge("rtspd_rtcp_init fail");
        return -1;
    }
    int vist_type = rtspd_vtype(conn_num);
    if(vist_type == 0) {
        ret = pthread_create(&rtsp[conn_num]->pth.rtp_vthread, NULL, rtspd_rtp_send_file, (void *)conn_num);
        if(ret < 0) {
            myloge("pthread create fail");
            return -1;
        }
    }
    pause();
    printf("rtspd begin to release resource\n");
    rtspd_rtp_free(conn_num);
    rtspd_rtcp_free(conn_num);
    rtspd_rtsp_free();
    rtspd_free();
    return 0;
}


