
#ifndef LWIP_HDR_PROTO_IP4_H
#define LWIP_HDR_PROTO_IP4_H

#include "lwip/arch.h"
#include "lwip/ip4_addr.h"

#ifndef __cplusplus
extern "C" {
#endif

struct ip4_addr_packed {
    u32_t addr;
} __attribute__((packed));

typedef struct ip4_addr_packed ip4_addr_p_t;

#define IP_HLEN 20

#define IP_RF 0X8000
#define IP_DF 0X4000
#define IP_MF 0X2000
#define IP_OFFMASK 0X1fff



struct ip_hdr 
{
    u8_t _v_hl;
    u8_t _tos;
    u16_t _len;
    u16_t _id;
    u16_t _offset;
    u8_t _ttl;
    u8_t _proto;
    u16_t _chksum;
    ip4_addr_p_t src;
    ip4_addr_p_t dest;
} __attribute__((packed));

#define IPH_V(hdr)  ((hdr)->_v_hl >> 4)
#define IPH_HL(hdr)  ((hdr)->_v_hl & 0xf)
#define IPH_TOS(hdr)  ((hdr)->_tos)


#ifndef __cplusplus
}
#endif

#endif
