#ifndef __RTCP_H__
#define __RTCP_H__

enum rtcp_pkt_type {
    SR = 200,
    RR = 201,
    SDES = 202,
    BYE = 203,
    APP = 204,
};

enum rtcp_info {
    CNAME=1,
    NAME=2,
    EMAIL = 3,
    PHONE = 4,
    LOC = 5,
    TOOL = 6,
    NOTE = 7,
    PRIV =8,
};



struct rtcp_header {
    u32 count:5;
    u32 padding:1;
    u32 version:2;

    u32 pt:8;
    u32 length:16;
};



struct rtcp_header_sdes {
    u32 ssrc;
    u8 attr_name;
    u8 len;
    char name[];
};


struct rtcp_pkt {
    struct rtcp_header comm;
    struct rtcp_header_sdes sdes;
};


#endif