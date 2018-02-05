#include "lwip/opt.h"


#include "lwip/def.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/raw.h"
#include "lwip/stats.h"
#include "lwip/inet_chksum.h"

#include <string.h>

static struct raw_pcb *raw_pcbs;

struct raw_pcb *raw_new(u8 proto)
{
    struct raw_pcb *pcb;
    pcb = (struct raw_pcb *)memp_malloc(MEMP_RAW_PCB);
    if(pcb != NULL)
    {
        memset(pcb, 0, sizeof(*pcb));
        pcb->protocol = proto;
        pcb->ttl = RAW_TTL;
        pcb->next = raw_pcbs;
        raw_pcbs = pcb;
    }
    return pcb;
}

void raw_recv
(
    struct raw_pcb *pcb,
    raw_recv_fn recv,
    void *recv_arg
)
{
    pcb->recv = recv;
    pcb->recv_arg = recv_arg;
}

struct raw_pcb * raw_new_ip_type
(
    u8_t type, 
    u8_t proto
)
{
    struct raw_pcb *pcb;
    pcb = raw_new(proto);
    return pcb;
}

err_t raw_sendto
(
    struct raw_pcb *pcb,
    struct pbuf *p,
    const ip_addr_t *ipaddr
)
{
    s16_t header_size;
    struct pbuf *q;
    struct netif *netif;
    ip_addr_t *src_ip;
    err_t err;
    header_size = IP_HLEN;
    
    if(pbuf_header(p, header_size) != 0)
    {
        //this means no space in pbuf 
        //always happens
        q = pbuf_alloc(PBUF_IP, 0, PBUF_RAM);
        if(q == NULL)
        {
            myloge("pbuf alloc failed");
            return ERR_MEM;
        }
        if(p->tot_len != 0)
        {
            pbuf_chain(q,p);
        }
        
    } 
    else
    {
        mylogd("TODO");
    }

    if((IP_IS_ANY_TYPE_VAL(pcb->local_ip)))
    {
        //never happens
    }
    else 
    {
        netif = ip_route(&pcb->local_ip, ipaddr);
    }

    if(netif == NULL)
    {
        if(q !=p )
        {
            pbuf_free(q);
            
        }
        myloge("can't find the right netif");
        return ERR_RTE;
    }

    if(ip_addr_isany(&pcb->local_ip))
    {
        
    }
    else
    {
        src_ip = &pcb->local_ip;
    }

    err = ip_output_if();
}
err_t raw_send
(
    struct raw_pcb *pcb,
    struct pbuf *p
)
{
    return raw_sendto(pcb, p, &pcb->remote_ip);
}

static u8_t raw_input_match
(
	struct raw_pcb *pcb,
	u8_t broadcast
)
{
	if(1)// ip version match 
	{
		if(broadcast!=0)
		{
			//todo
		}
		else
		{
			//if()
			return 1;
		}
	}
	return 0;
}
u8_t raw_input(struct pbuf *p, struct netif * inp)
{
	s16_t proto;
	struct raw_pcb *prev, *pcb;
	u8_t eaten = 0;
	u8_t broadcast = 0;
	proto = ((struct ip_hdr *)p->payload)->_proto;

	prev = NULL;
	pcb =raw_pcbs;

	while((eaten == 0) 
		&& (pcb != NULL))
	{
		if((pcb->protocol == proto)
			&&(raw_input_match(pcb, broadcast) ) 
			)
		{
			if(pcb->recv != NULL)
			{
				void *old_payload = p->payload;
				eaten = pcb->recv(pcb->recv_arg, pcb, p , &ip_data.current_iphdr_src);
				if(eaten != 0)
				{
					p = NULL;
					eaten = 1;
					if(prev != NULL)
					{
						prev->next = pcb->next;
						pcb->next = raw_pcbs;
						raw_pcbs = pcb;
					}
				}
				else
				{
					myloge("not eat packet");
				}
			}
		}

		prev = pcb;
		pcb = pcb->next;
	}
	return eaten;
}
