#include "rtsp.h"


struct rtsp_buffer *rtsp[MAX_CONN];

#define RTP_DEFAULT_PORT 5004
int start_port = RTP_DEFAULT_PORT;


extern sem_t rtspd_lock[MAX_CONN];
int rtsp_get_free_num(void)
{
    int i;
	pthread_mutex_lock(&rtspd_mutex);
	pthread_cond_wait(&rtspd_cond, &rtspd_mutex);
	pthread_mutex_unlock(&rtspd_mutex);
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
		mylogd("get options");
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
		mylogd("before send options reply");
        rtsp_send_reply_options(200, conn_num);
        return 0;
    }
    return -1;
}
int rtsp_parse_url(char *url, char *server, u16 *port, char *filename)
{
	char temp_url[128] = "";
	strcpy(temp_url, url);
	char *token, *port_str;
	int valid_url;
	if(strncmp(temp_url, "rtsp://", 7) == 0) {
		token = strtok(&temp_url[7], " :/\t\n");
		strcpy(server, token);
		port_str = strtok(&temp_url[strlen(server)]+7+1, " /\t\n");
		if(port_str) {
			*port = (u16) atol(port_str);
		} else {
			*port = 554;
		}
		valid_url = 1;
		token = strtok(NULL, " ");
		if(token) {
			strcpy(filename, token);
		} else {
			filename[0] = '\0';
		}
		
	} else {

	}
	if(valid_url) {
		return 0;
	}else {
		return -1;
	}
	
}
int rtsp_check_url(int conn_num)
{
	int ret;
	char url[128];
	
	ret = sscanf(rtsp[conn_num]->in_buffer, " %*s %254s ", url);
	if(ret == 0) {
		rtsp_send_reply(400, conn_num);
		return -1;
	}
	char object[128], server[128];
	u16 port;
	ret = rtsp_parse_url(url, server, &port, object);
	if(ret < 0) {
		rtsp_send_reply(400, conn_num);
		return -1;
	}
	strcpy(rtsp[0]->hostname, server);
	if(strstr(object, "trackID")) {
		strcpy(object, rtsp[0]->filename);
	} else {
		if(strcmp(object, "")==0) {
			strcpy(object, rtsp[0]->filename);
		} 
		strcpy(rtsp[0]->filename, object);
	}
	return 0;
}

int is_supported_mediatype(char *p, int conn_num)
{
	if(strcasecmp(p, ".264") == 0) {
		rtsp[conn_num]->vist_type = 0;
		return 1;
	}
	if(strcasecmp(p, ".H264") == 0) {
		rtsp[conn_num]->vist_type = 0;
		return 1;
	}
	if(strcasecmp(p, "h264stream") == 0) {
		rtsp[conn_num]->vist_type = 2;
		return 1;
	}
	if(strcasecmp(p, ".ps") == 0) {
		rtsp[conn_num]->vist_type = 1;
		return 1;
	}
	return 0;
	
}
int rtsp_check_filename(int conn_num)
{
	if(strstr(rtsp[0]->filename, "../")) {
		rtsp_send_reply(403, conn_num);
	}
	if(strstr(rtsp[0]->filename, "./")) {
		rtsp_send_reply(403, conn_num);
	}
	char *p;
	p = strrchr(rtsp[0]->filename, '.');
	int valid_url = 0;
	if(p == NULL) {
		rtsp_send_reply(415, conn_num);
		return -1;
	} else {
		valid_url = is_supported_mediatype(p, conn_num);
	}
	if(!valid_url) {
		rtsp_send_reply(415, conn_num);
		return -1;
	}
	return 0;
}

float ntp_time(time_t t)
{
	return (float)t+2208988800U;
}

int rtsp_get_describe_sdp(char *sdp_buf, int conn_num)
{
	char buf[30] = {0};
	strcpy(sdp_buf, "v=0"SDP_EL);
	strcat(sdp_buf, "o=");
	strcat(sdp_buf, PACKAGE);
	strcat(sdp_buf, " ");
	sprintf(buf, "%.0f", ntp_time(time(NULL)));
	strcat(sdp_buf, buf);//session id
	strcat(sdp_buf, " ");
	memset(buf, 0, 30);
	sprintf(buf, "%.0f", ntp_time(time(NULL)));
	strcat(sdp_buf, buf);//sdp VERSION
	strcat(sdp_buf, SDP_EL);
	strcat(sdp_buf, "c=");
	strcat(sdp_buf, "IN ");
	strcat(sdp_buf, "IP4 ");
	strcat(sdp_buf, rtsp[0]->hostname);
	strcat(sdp_buf, SDP_EL);
	strcat(sdp_buf, "s=RTSP Session"SDP_EL);
	sprintf(sdp_buf+  strlen(sdp_buf), "i=%s %s Streaming Server"SDP_EL, PACKAGE, VERSION);
	sprintf(sdp_buf+ strlen(sdp_buf), "u=%s"SDP_EL, rtsp[0]->filename);
	strcat(sdp_buf, "t=0 0"SDP_EL);

	strcat(sdp_buf, "m=");
	strcat(sdp_buf, "video 0");
	strcat(sdp_buf, " RT/VAP ");
	rtsp[conn_num]->payload_type = 96;
	sprintf(sdp_buf + strlen(sdp_buf), "%d"SDP_EL, rtsp[conn_num]->payload_type);
	if(rtsp[conn_num]->payload_type >= 96) {
		strcat(sdp_buf, "a=rtpmap:");
		sprintf(sdp_buf+strlen(sdp_buf), "%d", rtsp[conn_num]->payload_type);
		strcat(sdp_buf, " ");
		strcat(sdp_buf, "H264/90000");
		strcat(sdp_buf, SDP_EL);
		strcat(sdp_buf, "a=fmtp:96 packetization-mode=1;profile-level-id=1EE042;sprop-parameter-sets=QuAe2gLASRA=,zjCkgA==");
		strcat(sdp_buf, SDP_EL);
		strcat(sdp_buf, "a=control:");
		sprintf(sdp_buf+strlen(sdp_buf), "rtsp://%s/%s/trackID=0", rtsp[0]->hostname,rtsp[0]->filename);
		strcat(sdp_buf, SDP_EL);
	}
	strcat(sdp_buf, SDP_EL);
	return 0;
	
}

/**
	crlf : CR LF
*/
void add_timestamp(char *buf, int crlf)
{
	struct tm *t;
	time_t now;
	now = time(NULL);
	t = gmtime(&now);
	strftime(buf+strlen(buf), 38, "Date: %a, %d %b %Y %H:%M:%S GMT"RTSP_EL, t);
	if(crlf) {
		strcat(buf, "\r\n");
	}
	
}
int rtsp_send_describe_reply(int status, int conn_num)
{
	char *p = &rtsp[conn_num]->out_buffer[0];
	sprintf(p, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s"RTSP_EL, 
			RTSP_VER, status, rtsp_get_stat(status), 
			rtsp[conn_num]->rtsp_cseq, PACKAGE, VERSION);
	add_timestamp(p, 0);
	strcat(p, "Content-Type: application/sdp"RTSP_EL);
	sprintf(p + strlen(p), "Content-Base: rtsp://%s/%s/"RTSP_EL, 
		rtsp[0]->hostname, rtsp[0]->filename);
	sprintf(p+strlen(p), "Content-Length: %d"RTSP_EL, strlen(rtsp[conn_num]->sdp_buffer));
	strcat(p, RTSP_EL);
	strcat(p, rtsp[conn_num]->sdp_buffer);
	int ret;
	ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, rtsp[conn_num]->out_buffer, strlen(rtsp[conn_num]->out_buffer));
	if(ret <= 0) {
		myloge("write fail");
		return -1;
	}
	return 0;
}
int rtsp_method_describe(int conn_num)
{
	int ret;
	ret = rtsp_check_url(conn_num);
	if(ret) {
		return -1;
	}
	ret = rtsp_check_filename(conn_num);
	if(ret) {
		return -1;
	}
	if(strstr(rtsp[conn_num]->in_buffer, HDR_REQUIRE)) {
		rtsp_send_reply(551, conn_num);
		return -1;
	}
	if(strstr(rtsp[conn_num]->in_buffer, HDR_ACCEPT)!=NULL) {
		if(strstr(rtsp[conn_num]->in_buffer, "application/sdp") ) {
			;
		} else {
			rtsp_send_reply(551, conn_num);
			return -1;
		}
	}
	if(rtsp_get_cseq(conn_num) < 0) {
		return -1;
	}	
	if(rtsp_get_describe_sdp(rtsp[conn_num]->sdp_buffer, conn_num)<0) {
		return -1;
	}
	if(rtsp_send_describe_reply(200, conn_num) == 0) {
		return 0;
	}
	return -1;
}

void rtsp_get_server_port(int conn_num)
{
	rtsp[conn_num]->cmd_port.rtp_ser_port = 
		(rtsp[conn_num]->cmd_port.rtp_cli_port-start_port)/2+start_port;
	rtsp[conn_num]->cmd_port.rtcp_ser_port = rtsp[conn_num]->cmd_port.rtp_ser_port + 1;
	
}

u64 rtsp_get_random_seq(void)
{
	u64 seed;
	srand(time(NULL));
	seed = 1+ (u32)(rand()%(0xffff));
	return seed;
}

int rtsp_send_setup_reply(int status, int conn_num)
{
	char temp[40] = {0};
	char ttl[40] = {0};
	char *p = rtsp[conn_num]->out_buffer;
	sprintf(p, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s"RTSP_EL,
		RTSP_VER, status, rtsp_get_stat(status), 
		rtsp[conn_num]->rtsp_cseq, PACKAGE, VERSION);
	add_timestamp(p, 0);
	strcat(p, "Session: ");
	sprintf(temp, "%d", rtsp[conn_num]->session_id);
	strcat(p, temp);
	strcat(p, RTSP_EL);
	strcat(p, "Transport: RTP/AVP;unicast;client_port=");
	sprintf(temp, "%d", rtsp[conn_num]->cmd_port.rtp_cli_port);
	strcat(p, temp);
	strcat(p, "-");
	sprintf(temp, "%d", rtsp[conn_num]->cmd_port.rtcp_cli_port);
	strcat(p, temp);

	strcat(p, rtsp[0]->hostname);
	strcat(p, temp);
	strcat(p, ";server_port=");
	sprintf(temp, "%d", rtsp[conn_num]->cmd_port.rtp_ser_port);
	strcat(p, temp);
	strcat(p, "-");
	sprintf(temp, "%d", rtsp[conn_num]->cmd_port.rtcp_ser_port);
	strcat(p, temp);

	sprintf(temp, ";ssrc=%u", rtsp[conn_num]->cmd_port.ssrc);
	strcat(p, temp);
	strcat(p, ";ttl=");
	sprintf(ttl, "%d", 32);
	strcat(p, ttl);
	strcat(p, RTSP_EL);
	strcat(p, RTSP_EL);

	int ret;
	ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, rtsp[conn_num]->out_buffer, 
		strlen(rtsp[conn_num]->out_buffer));
	if(ret < 0) {
		myloge("write fail");
		return -1;
	}
	return 0;
}
int rtsp_method_setup(int conn_num)
{
	int ret;
	ret = rtsp_check_url(conn_num);
	if(ret < 0) {
		return -1;
	}
	ret = rtsp_check_filename(conn_num);
	if(ret < 0) {
		return -1;
	}
	ret = rtsp_get_cseq(conn_num);
	if(ret < 0) {
		return -1;
	}
	char *p;
	p = strstr(rtsp[conn_num]->in_buffer, "client_port");
	if(p == NULL) {
		myloge("");
		rtsp_send_reply(406, conn_num);
		return -1;
	}
	p = strstr(rtsp[conn_num]->in_buffer, HDR_TRANSPORT);
	if(p==NULL) {
		myloge("");
		rtsp_send_reply(406, conn_num);
		return -1;
	}
	char trash[255], line[255];
	if(sscanf(p, "%10s%255s", trash, line) != 2) {
		rtsp_send_reply(400, conn_num);
		return -1;
	}
	if(strstr(line, "client_port" ) != NULL) {
		p = strstr(line, "client_port");
		p = strstr(p, "=");
		sscanf(p+1, "%d", &(rtsp[conn_num]->cmd_port.rtp_cli_port));
		p = strstr(p, "-");
		sscanf(p+1, "%d", &(rtsp[conn_num]->cmd_port.rtcp_cli_port));
		
	}
	rtsp_get_server_port(conn_num);
	rtsp[conn_num]->cmd_port.seq = rtsp_get_random_seq();
	rtsp[conn_num]->cmd_port.ssrc = random32(0);
	rtsp[conn_num]->cmd_port.timestamp = random32(0);
	ret = rtsp_send_setup_reply(200, conn_num);
	if(ret) {
		return -1;
	}
	return 0;
}

int rtsp_send_play_reply(int status, int conn_num)
{
	char temp[255];
	char *p = rtsp[conn_num]->out_buffer;
	sprintf(p, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s",
		RTSP_VER, status, rtsp_get_stat(status), 
		rtsp[conn_num]->rtsp_cseq, PACKAGE,VERSION );
	add_timestamp(p, 0);
	strcat(p, "Session: ");
	sprintf(temp, "%d", rtsp[conn_num]->session_id);
	strcat(p, temp);
	strcat(p, RTSP_EL);
	strcat(p, RTSP_EL);
	int ret;
	ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, 
		rtsp[conn_num]->out_buffer, strlen(rtsp[conn_num]->out_buffer));
	if(ret < 0) {
		myloge("write fail");
		return -1;
	}
	return 0;
}
int rtsp_method_play(int conn_num)
{
	int ret;
	ret |= rtsp_check_url(conn_num);
	ret |= rtsp_get_cseq(conn_num);
	ret |= rtsp_check_filename(conn_num);
	
	if(ret ) {
		myloge("");
		return -1;
	}
	char *p;
	char *inbuf = rtsp[conn_num]->in_buffer;
	p = strstr(inbuf, HDR_SESSION);
	char trash[255] = {0};
	if(p != NULL) { // found
		ret = sscanf(p, "%254s %d", trash, &rtsp[conn_num]->session_id);
		if(ret != 2) {
			rtsp_send_reply(454, conn_num);
			return -1;
		}
	} else {
		rtsp_send_reply(400, conn_num);
	}
	ret = rtsp_send_play_reply(200, conn_num);
	if(ret ) {
		return -1;
	}
	return 0;
}
int rtsp_send_teardown_reply(int status, int conn_num)
{
	char *outbuf = rtsp[conn_num]->out_buffer;
	sprintf(outbuf, "%s %d %s"RTSP_EL"CSeq: %d"RTSP_EL"Server: %s/%s",
		RTSP_VER, status, rtsp_get_stat(status), 
		rtsp[conn_num]->rtsp_cseq, 
		PACKAGE, VERSION);
	add_timestamp(outbuf, 0);
	strcat(outbuf, "Session: ");
	char temp[255];
	sprintf(temp, "%d", rtsp[conn_num]->session_id);
	strcat(outbuf, temp);
	strcat(outbuf, RTSP_EL);
	strcat(outbuf, RTSP_EL);
	int ret;
	ret = write(rtsp[conn_num]->cli_rtsp.cli_fd, 
		rtsp[conn_num]->out_buffer,
		strlen(rtsp[conn_num]->out_buffer));
	if(ret < 0) {
		myloge("write fail");
		return -1;
	}
	return 0;
}
int rtsp_method_teardown(int conn_num)
{
	int ret;
	ret |= rtsp_check_url(conn_num);
	ret |= rtsp_get_cseq(conn_num);
	ret |= rtsp_check_filename(conn_num);
	
	if(ret ) {
		return -1;
	}
	char *p;
	char *inbuf = rtsp[conn_num]->in_buffer;
	p = strstr(inbuf, HDR_SESSION);
	char trash[255] = {0};
	if(p != NULL) { // found
		ret = sscanf(p, "%254s %d", &rtsp[conn_num]->session_id);
		if(ret != 2) {
			rtsp_send_reply(454, conn_num);
			return -1;
		}
	} else {
		rtsp_send_reply(400, conn_num);
	}
	ret = rtsp_send_teardown_reply(200, conn_num);
	if(ret ) {
		return -1;
	}
	return 0;
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
				sem_post(&rtspd_lock[conn_num]);
				return -1;
            } else {
                mylogd("options ok");
                rtsp[conn_num]->rtspd_status = 0x02;
            }
            break;
		case 2:
			ret = rtsp_method_describe(conn_num);
			if(ret < 0) {
				myloge("rtsp_method_describe fail");
				//close();
				rtsp[conn_num]->rtspd_status = 0x03;
				sem_post(&rtspd_lock[conn_num]);
				return -1;
			} else {
				mylogd("describe ok");
				rtsp[conn_num]->rtspd_status = 0x04;
			}
			break;
		case 3:
			ret = rtsp_method_setup(conn_num);
			if(ret < 0) {
				myloge("setup fail");
				close(rtsp[conn_num]->cli_rtsp.cli_fd);
				rtsp[conn_num]->rtspd_status = 0x05;
				rtsp[conn_num]->is_running = 0;
				sem_post(&rtspd_lock[conn_num]);
				return -1;
			} else {
				mylogd("setup ok");
				rtsp[conn_num]->rtspd_status = 0x06;
			}
			break;
		case 4:
			ret = rtsp_method_play(conn_num);
			if(ret < 0) {
				myloge("play fail");
				close(rtsp[conn_num]->cli_rtsp.cli_fd);
				rtsp[conn_num]->rtspd_status = 0x07;
				rtsp[conn_num]->is_running = 0;
				sem_post(&rtspd_lock[conn_num]);
				return -1;
			} else {
				mylogd("play ok");
				rtsp[conn_num]->is_running = 1;
				rtsp[conn_num]->rtspd_status = 0x08;
				sem_post(&rtspd_lock[conn_num]);
			}
			break;
		case 5:
			ret = rtsp_method_teardown(conn_num);
			if(ret < 0) {
				myloge("teardown fail");
				close(rtsp[conn_num]->cli_rtsp.cli_fd);
				rtsp[conn_num]->is_running = 0;
				rtsp[conn_num]->rtspd_status = 0x11;
				return -1;
			} else {
				mylogd("teardown ok");
				rtsp[conn_num]->is_running = 0;
				close(rtsp[conn_num]->cli_rtsp.cli_fd);
				rtsp[conn_num]->rtspd_status = 0x12;
			}
		default:
			myloge("no such method");
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
		mylogd("before tcp read,fd:%d",fd);
        ret = socket_tcp_read(fd, 
			rtsp[conn_num]->in_buffer, 4096);
        if(ret < 0) {
			myloge("socket_tcp_read fail");
            break;
        }
        int method;
        method = rtsp_get_method(conn_num);
		mylogd("method:%d",method);
        ret = rtsp_proc_method(method, conn_num);
        if(ret < 0) {
			rtsp[conn_num]->conn_status = 0;
        } else {
			rtsp[conn_num]->conn_status = 1;
		}
    }
	mylogd("rtsp main loop exit");
    pthread_exit(NULL);
}