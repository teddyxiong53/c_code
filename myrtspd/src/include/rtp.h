#ifndef __RTP_H__
#define __RTP_H__

#define RTP_SIZE_MAX 1460
#define RTP_HEADER_SIZE 12
#define NALU_INDIC_SIZE 4
#define NALU_HEADER_SIZE 1
#define FU_A_INDI_SIZE 1
#define FU_A_HEAD_SIZE 1

#define SINGLE_NALU_DATA_MAX 1448 
#define SLICE_NALU_DATA_MAX 1446

#define MIN(x,y) ((x)>(y)?(y):(x))

#define READ_LEN 150000
#define SLICE_SIZE 1448
#define SLICE_FSIZE 1435
#define DE_TIME 3600

struct rtp_header {
    //byte0
    u8 csrc_len:4;
    u8 extension:1;
    u8 padding:1;
    u8 version:2;

    //byte1
    u8 payload:7;
    u8 marker:1;

    u16 seq_no;
    u32 timestamp;
    u32 ssrc;
};



#endif