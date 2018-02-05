#include "lwip/opt.h"


#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/ip.h"
#include "lwip/def.h"
#include "lwip/stats.h"

#include <string.h>

void icmp_input(struct pbuf *p, struct netif *inp)
{
	u8 type;
	struct ip_hdr *iphdr_in;
	u16_t hlen;
	struct icmp_echo_hdr *iecho;
	ip4_addr_t *src;
	iphdr_in = ip_data.current_ip4_header;
	hlen = IPH_HL(iphdr_in)*4;
	if(hlen < IP_HLEN)
	{
		myloge("ip header len too short");
		goto lenerr;
	}
	if(p->len < sizeof(u16_t)*2)
	{
		myloge("icmp too short");
		goto lenerr;
	}
	type = *((u8_t *)p->payload);
	switch(type)
	{
		case ICMP_ER:
			break;
		case ICMP_ECHO:
			src = &ip_data.current_iphdr_dest;
			if(ip4_addr_ismulticast(src))
			{
				goto icmperr;
			}
			iecho = (struct icmp_echo_hdr *)p->payload;
			if(pbuf_header(p, (s16_t)hlen))
			{
				//fail
			}
			else
			{
				struct ip_hdr *iphdr = (struct ip_hdr)p->payload;
				ip4_addr_copy(iphdr->src, *src);
				ip4_addr_copy(iphdr->dest, ip_data.current_iphdr_src);
				iecho->type = ICMP_ER;
				iecho->chksum = 0;
				iphdr->_ttl = ICMP_TTL;
				iphdr->_chksum = 0;
				iphdr->_chksum = inet_chksum(iphdr, hlen);
				err_t ret;
				ret = ip4_output_if(p, src, LWIP_IP_HDRINCL, ICMP_TTL, 0, IP_PROTO_ICMP, inp);
			}
			break;
	}
	
	pbuf_free(p);
	return;
lenerr:
	pbuf_free(p);
	return;
icmperr:
	pbuf_free(p);
	return;
}

