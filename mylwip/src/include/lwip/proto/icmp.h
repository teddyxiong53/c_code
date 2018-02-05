#ifndef LWIP_HDR_PROTO_ICMP_H
#define LWIP_HDR_PROTO_ICMP_H

#include "lwip/arch.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ICMP_ER 0//echo reply
#define ICMP_DUR 3//dest unreachable
#define ICMP_SQ 4//source quech(die)
#define ICMP_RD 5//redirect
#define ICMP_ECHO 8// echo
#define ICMP_TE 11//time exceeded
#define ICMP_PP 12//param problem


#define ICMP_TS 13//timestramp
#define ICMP_TSR 14//timestamp reply
#define ICMP_IRQ 15//info request
#define ICMP_IR 16//info reply
#define ICMP_AM 17//address mask request
#define ICMP_AMR 18//address mask reply

struct icmp_echo_hdr
{
	u8 type;
	u8 code;
	u16 chksum;
	u16 id;
	u16 seqno;
}__attribute__((packed));





#ifdef __cplusplus
}
#endif

#endif


