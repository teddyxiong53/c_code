#include "rtsp.h"
#include "rtcp.h"
#include "rtp.h"

u64 get_file_size(FILE *file)
{
    fseek(file, 0, SEEK_END);
    int sz;
    sz = ftell(file);
    fseek(file, 0, SEEK_SET);
    return sz;
}

/**
 * Abstract the NALU Indicator bytes(00 00 00 01)
 */

int abstr_nalu_indic(char *buf, int buf_size, int *found)
{
    int frame_size = 4;
    char *p = buf + 4;
    int offset = 0;
    *found = 0;

    while(frame_size < buf_size -4) {
        if(p[2]) {
            offset = 3;
        } else if(p[1]) {
            offset = 2;
        } else if(p[0]) {
            offset = 1;
        } else {
            if(p[3] != 1) {
                if(p[3]) {
                    offset = 4;
                } else {
                    offset = 1;
                }
            } else {
                *found = 1;
                break;
            }
        }
        frame_size += offset;
        p += offset;
    }
    if(!*found) {
        frame_size = buf_size;
    }
    return frame_size;
}

int build_rtp_header(struct rtp_header *hdr, int conn_num)
{
    hdr->version = 2;
    hdr->padding = 0;
    hdr->extension = 0;
    hdr->csrc_len = 0;
    hdr->marker = 0;
    hdr->payload = 96;
    hdr->seq_no = htons(rtsp[conn_num]->cmd_port.seq);
    rtsp[conn_num]->cmd_port.timestamp += rtsp[conn_num]->cmd_port.frame_rate_step;
    hdr->timestamp = htonl(rtsp[conn_num]->cmd_port.timestamp);
    hdr->ssrc = htonl(rtsp[conn_num]->cmd_port.ssrc);
	return 0;
}

int udp_write(int len, int conn_num)
{
	int ret;
	ret = write(rtsp[conn_num]->fd.video_rtp_fd, rtsp[conn_num]->nalu_buffer, len);
	if(ret <=0 ) {
		rtsp[conn_num]->rtspd_status = 0x21;
	}
	return 0;
}

int build_rtp_nalu(u8 *buf, int frame_size, int conn_num)
{
    char *nalu_buffer;
    nalu_buffer = rtsp[conn_num]->nalu_buffer;
    struct rtp_header rtp_hdr;
	int fu_end;
    char fu_header;
    int fu_start;
	char nalu_header;
	char fu_indic;
    build_rtp_header(&rtp_hdr, conn_num);
    int data_left = frame_size - NALU_INDIC_SIZE;
    char *p_nalu_data = buf + NALU_INDIC_SIZE;
    if(data_left <= SINGLE_NALU_DATA_MAX) {
        rtp_hdr.seq_no = htons(rtsp[conn_num]->cmd_port.seq++);
        rtp_hdr.marker = 1;
        memcpy(nalu_buffer, &rtp_hdr, sizeof(rtp_hdr));
        memcpy(nalu_buffer + RTP_HEADER_SIZE, p_nalu_data, data_left);
        udp_write(data_left+ RTP_HEADER_SIZE, conn_num);
        usleep(DE_TIME);
        return 0;
    }
    nalu_header = buf[4];
    fu_indic = (nalu_header & 0xe0) | 28;
    data_left -= NALU_HEADER_SIZE;
    p_nalu_data += NALU_HEADER_SIZE;
    
    while(data_left > 0) {
        int proc_size = MIN(data_left, SLICE_NALU_DATA_MAX);
        int rtp_size = proc_size + RTP_HEADER_SIZE + FU_A_HEAD_SIZE 
            + FU_A_INDI_SIZE;
        if(proc_size == data_left) {
            fu_end = 1;
        } else {
            fu_end = 0;
        }
        fu_header = nalu_header &0x1f;
        if(fu_start) {
            fu_header |= 0x80;
        } else if(fu_end) {
            fu_header |= 0x40;
        }
        rtp_hdr.seq_no = htons(rtsp[conn_num]->cmd_port.seq++);
		memcpy(nalu_buffer, &rtp_hdr, sizeof(rtp_hdr));
		memcpy(nalu_buffer + 14, p_nalu_data, proc_size);
		nalu_buffer[12] = fu_indic;
		nalu_buffer[13] = fu_header;
		udp_write(rtp_size, conn_num);
		if(fu_end) {
			usleep(36000);
		}
		data_left -= proc_size;
		p_nalu_data += proc_size;
		fu_start = 0;
    }
	return 0;
}
int rtp_send_packet(int conn_num )
{
    FILE *infile;
	mylogd("filename:%s", rtsp[0]->filename);
    infile = fopen(rtsp[0]->filename, "rb");
    if(!infile) {
        myloge("fopen fail");
        return -1;
    }
    u64 filesize = get_file_size(infile);
    mylogd("filesize:%llu", filesize);

    u8 inbuf[READ_LEN] = "";
    u8 outbuf[READ_LEN] = "";

    int bytes_left;
    char *p;
    int frame_size;
    int found_nalu;
    int reach_last_nalu = 0;
    int bytes_consumed = 0;
    int total_size;
    while(rtsp[conn_num]->is_running) {
        bytes_left = fread(inbuf, 1, READ_LEN, infile);
        p = inbuf;
        while(bytes_left > 0) {
            frame_size = abstr_nalu_indic(p, bytes_left, &found_nalu);
            if(bytes_consumed + frame_size >= total_size) {
                reach_last_nalu = 1;
            } else {
                reach_last_nalu = 0;
            }
            if(found_nalu || reach_last_nalu) {
                memcpy(outbuf, p, frame_size);
                build_rtp_nalu(outbuf, frame_size, conn_num);
                p += frame_size;
                bytes_consumed += frame_size;
                if(reach_last_nalu) {
                    rtsp[conn_num]->is_running = 0;
                }
            }
            bytes_left -= frame_size;
        }
        fseek(infile, bytes_consumed, SEEK_SET);
    }
    fclose(infile);
    close(rtsp[conn_num]->fd.video_rtp_fd);
    return 0;
}
