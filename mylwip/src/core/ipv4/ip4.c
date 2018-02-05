#include "lwip/opt.h"


#include "lwip/ip.h"
#include "lwip/def.h"
#include "lwip/mem.h"
//#include "lwip/ip4_frag.h"
#include "lwip/inet_chksum.h"
#include "lwip/netif.h"
#include "lwip/icmp.h"
//#include "lwip/igmp.h"
#include "lwip/raw.h"
//#include "lwip/udp.h"
//#include "lwip/priv/tcp_priv.h"
//#include "lwip/autoip.h"
//#include "lwip/stats.h"
//#include "lwip/prot/dhcp.h"

#include <string.h>

#define LWIP_INLINE_IP_CHKSUM   1
#define CHECKSUM_GEN_IP_INLINE  1

static u16_t ip_id;

struct netif * ip4_route(struct ip4_addr_t *dest)
{
    struct netif *netf;
    for(netif=netif_list; netif!=NULL; netif=netif->next)
    {
        if(
            netif_is_up(netif)
            && (netif_is_link_up(netif))
            && !(ip4_addr_isany_val(netif->ip_addr))
            )
        {
            if(ip4_addr_netcmp(dest, netif->ip_addr, netif->netmask))
            {
                return netif;
            }
            //if()
        }
    }
}


err_t ip4_output_if
(
	struct pbuf *p,
	ip4_addr_t *src,
	ip4_addr_t *dest,
	u8_t ttl,
	u8_t tos,
	u8_t proto,
	struct netif *netif
)
{
	return ip4_output_if_opt(p, src, dest, ttl, tos, proto, netif, NULL, 0);
}

err_t ip4_output_if_opt
(
	struct pbuf *p,
	ip4_addr_t *src,
	ip4_addr_t *dest,
	u8_t ttl,
	u8_t tos,
	u8_t proto,
	struct netif *netif,
	void *ip_options,
	u16_t optlen
)
{
	ip4_addr_t *src_used = src;
	if(dest != LWIP_IP_HDRINCL)// means != null
	{
		if(ip4_addr_isany(src))
		{
			src_used = &netif->ip_addr;
		}
	}
	return ip4_output_if_opt_src(p, src_used, dest, ttl, tos, proto, ip_options, optlen);
}

err_t ip4_output_if_opt_src
(
	struct pbuf *p,
	ip4_addr_t *src,
	ip4_addr_t *dest,
	u8_t ttl,
	u8_t tos,
	u8_t proto,
	struct netif *netif,
	void *ip_options,
	u16_t optlen
)
{
	struct ip_hdr *iphdr;
	ip4_addr_t dest_addr;
	u32_t chk_sum = 0;
	if(dest != LWIP_IP_HDRINCL)//!=null
	{
		u16_t ip_hlen = IP_HLEN;
		if(pbuf_header(p, IP_HLEN)!=0)
		{
			//error
			myloge("pbuf_header failed");
			return ERR_BUF;
		}
		iphdr = (struct ip_hdr *)p->payload;
		//set ttl
		iphdr->_ttl = ttl;
		iphdr->_proto = proto;
		chk_sum += PP_HTONS(proto | (ttl << 8));
		ip4_addr_copy(iphdr->dest, *dest);

		chk_sum += iphdr->dest.addr & 0xffff;
		chk_sum += iphdr->dest.addr >> 16;

		iphdr->_v_hl = (u8_t )((4<<4) | ip_hlen/4);
		iphdr->_tos = tos;

		chk_sum += PP_NTOHS(tos | (iphdr->_v_hl<<8));
		iphdr->_len = lwip_htons(p->tot_len);

		chk_sum += iphdr->_len;
		iphdr->_offset = 0;

		iphdr->_id = lwip_htons(ip_id);
		chk_sum += iphdr->_id;

		ip_id ++;

		if(src == NULL)
		{
			ip4_addr_copy(iphdr->src, *IP4_ADDR_ANY);
		}
		else
		{
			ip4_addr_copy(iphdr->src, *src);
		}

		//calc checksum
		chk_sum += iphdr->src.addr & 0xffff;
		chk_sum += iphdr->src.addr >> 16;
		chk_sum = (chk_sum >> 16) + (chk_sum & 0xffff);
		chk_sum = (chk_sum >> 16) + chk_sum;
		chk_sum = ~chk_sum;

		iphdr->_chksum = (u16_t)chk_sum;
		
	}
	else
	{
		iphdr = (struct ip_hdr *)p->payload;
		ip4_addr_copy(dest_addr, iphdr->dest);
		dest = &dest_addr;
	}
	
	if(ip4_addr_cmp(dest, netif->ip_addr))
	{
		//so this is the loop netif
		return netif_loop_output(netif, p);
	}

	return netif->output(netif, p, dest);
}

struct netif *ip4_route_src(ip4_addr_t *dest, ip4_addr_t *src)
{
    return ip4_route(dest);
}



err_t ip4_input(struct pbuf *p, struct netif *inp)
{
	struct ip_hdr *iphdr;
	struct netif *netif;
	u16_t iphdr_hlen, iphdr_len;

	iphdr = (struct ip_hdr*)p->payload;
	if(IPH_V(iphdr) != 4)
	{
		myloge("ip version is not 4");
		pbuf_free(p);
		return ERR_OK;
	}
	iphdr_hlen = IPH_HL(iphdr);
	iphdr_hlen *= 4;
	iphdr_len = lwip_ntohs(iphdr->_len);

	if(iphdr_len < p->tot_len)
	{
		//for packet less than 60 bytes
		pbuf_realloc(p, iphdr_len);
	}

	if((iphdr_hlen > p->len)
		||(iphdr_len > p->tot_len) 
		||(iphdr_hlen < IP_HLEN)
		)
	{
		if(iphdr_hlen < IP_HLEN)
		{
			myloge("iphdr is too short");
		}
		if(iphdr_hlen > p->len)
		{
			myloge("ip hdr len is not all in the first pbuf, this is not normal");
		}
		if((iphdr_len > p->tot_len) )
		{
			myloge("ip packet len != pbuf payload len, not normal");
		}
		pbuf_free(p);
		return ERR_OK;
	}
	//not check the checksum
	ip_addr_copy(ip_data.current_iphdr_dest, iphdr->dest);
	ip_addr_copy(ip_data.current_iphdr_src, iphdr->src);

	if(0)//is multicast
	{
		//todo
	}
	else
	{
		int first = 1;
		netif = inp;
		do
		{
			if(netif_is_up(netif))
			{
				//todo
			}
			if(first)
			{
				if()//is loopback
				{
					netif = NULL;
					break;
				}
				first = 0;
				netif = netif_list;
			}
			else
			{
				netif = netif->next;
			}
			if(netif == inp)
			{
				netif = netif->next;
			}
		}while(netif!=NULL);
	}

	if(netif == NULL)//not for me
	{
		
	}

	if(raw_input(p, inp) == 0)
	{
		switch()
		{
			case IP_PROTO_UDP:
				break;
			default:
				break;
		}
	}
}
