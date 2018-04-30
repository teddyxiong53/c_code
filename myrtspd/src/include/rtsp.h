#ifndef __RTSP_H__
#define __RTSP_H__

#include "type.h"
#include "mylog.h"
#include <pthread.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <pthread.h>
#include <sys/utsname.h>
#include <unistd.h>
#include <errno.h>
#include <semaphore.h>
#define MAX_CONN 2

#define RTSP_EL "\r\n"
#define RTSP_VER "RTSP/1.0"


struct rtsp_port {
    s32 rtp_cli_port;
    s32 rtcp_cli_port;
    s32 rtp_ser_port;
    s32 rtcp_ser_port;
    u32 ssrc;
    u32 timestamp;
    u32 frame_rate_step;
    u16 seq;
};

struct rtsp_fd {
    s32 rtspfd;
    s32 video_rtp_fd;
    s32 video_rtcp_fd;
    s32 audio_rtp_fd;
    s32 audio_rtcp_fd;
};

struct rtsp_th {
    pthread_t rtsp_vthread;
    pthread_t rtsp_vthread1;
    pthread_t rtp_vthread;
    pthread_t rtcp_vthread;
};

struct rtsp_cli {
    s32 cli_fd;
    s32 conn_num;
    char cli_host[128];
};


struct rtsp_buffer {
    u32 payload_type;
    u32 session_id;
    u32 rtsp_deport;
    u32 rtsp_um_stat;// 0:unicast, 1:multicast;
    u32 rtsp_cseq;
    u32 is_running;
    u32 cur_conn;
    u32 conn_status;
    u32 rtspd_status;
    u32 vist_type; //0: h264 file vist , 1: ps file , 2: h264 stream

    struct rtsp_port cmd_port;
    struct rtsp_fd fd;
    struct rtsp_th pth;
    struct rtsp_cli cli_rtsp;
    char filename[128];
    char hostname[128];
    u8 nalu_buffer[1448];
    char in_buffer[4096];
    char out_buffer[4096];
    char sdp_buffer[4096];

};

extern struct rtsp_buffer *rtsp[MAX_CONN];

int rtsp_init_buffer(void);

#endif //__RTSP_H__