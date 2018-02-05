#include "lwip/opt.h"

#include <string.h>

#include "lwip/def.h"
#include "lwip/ip_addr.h"
//#include "lwip/ip6_addr.h"
#include "lwip/netif.h"
//#include "lwip/priv/tcp_priv.h"
//#include "lwip/udp.h"
//#include "lwip/raw.h"

//#include "lwip/etharp.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
//#include "lwip/ip.h"
#include "lwip/tcpip.h"
#include "lwip/pbuf.h"

//#include "netif/ethernet.h"

struct netif *netif_list;
struct netif *netif_default;

static struct netif loop_netif;
static u8_t netif_num;

void netif_set_ipaddr
(
    struct netif *netif,
    const ip4_addr_t *ipaddr
)
{
    ip4_addr_t new_addr = {0};
    new_addr = (ipaddr?*ipaddr:*IP4_ADDR_ANY);
    if(ip4_addr_cmp(&new_addr, netif_ip4_addr(netif)) == 0)
    {
        // this means not the same 
        //raw_netif_ip_addr_changed();
        ip4_addr_set(&netif->ip_addr, ipaddr);
        //netif_issue_reposrts(netif, NETIF_REPORT_TYPE_V4);
        
    }
}

void netif_set_netmask
(
    struct netif *netif,
    const ip4_addr_t *netmask
)
{

    ip4_addr_set(&netif->netmask, netmask);

}
void netif_set_gw
(
    struct netif *netif,
    const ip4_addr_t *gw
)
{

    ip4_addr_set(&netif->gw, gw);

}

void netif_set_addr
(
    struct netif *netif, 
    const ip4_addr_t *ipaddr, 
    const ip4_addr_t *netmask,
    const ip4_addr_t *gw
)
{
    if(ip4_addr_isany(ipaddr))//this means ipaddr is 0 or null
    {
        netif_set_ipaddr(netif, ipaddr);
        netif_set_netmask(netif, netmask);
        netif_set_gw(netif, gw);
    }
    else 
    {
        //change gw and netmask before ip
        netif_set_netmask(netif, netmask);
        netif_set_gw(netif, gw);
        netif_set_ipaddr(netif ,ipaddr);
    }
}
struct netif *netif_add(struct netif *netif,
          const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
          void *state, netif_init_fn init, netif_input_fn input)
{
    netif->loop_first = NULL;
    netif->loop_last = NULL;
    netif->state = state;
    netif->num = netif_num++;
    netif->input = input;
    netif_set_addr(netif, ipaddr, netmask, gw);
    if(init(netif)!= ERR_OK)
    {
        myloge("init netif failed \n");
        return NULL;
    }
    netif->next = netif_list;
    netif_list = netif;
    mylogd("add interface[%c%c] successfully \n", netif->name[0], netif->name[1]);
    return netif;
}
void netif_set_default(struct netif *netif)
{

}
void netif_set_up(struct netif *netif)
{


}
void netif_set_link_up(struct netif *netif)
{
    
}

err_t netif_loop_output(struct netif *netif, struct pbuf *p)
{
    struct pbuf *r;
    err_t err;
    struct pbuf *last;
    SYS_ARCH_DECL_PROTECT(lev);

    r = pbuf_alloc(PBUF_LINK, p->tot_len, PBUF_RAM);
    if(r == NULL)
    {
        myloge("pbuf alloc failed");
        return ERR_MEM;
    }
    err = pbuf_copy(r, p);
    if(err)
    {
        pbuf_free(r);
        myloge("pbuf_free failed ");
        return err;
    }
    //move last to the end of r 
    for(last=r; last->next!=NULL; last=last->next);
    SYS_ARCH_PROTECT(lev);
    if(netif->loop_first != NULL)
    {
        netif->loop_last->next = r;
        netif->loop_last = last;
    }
    else 
    {
        netif->loop_first = r;
        netif->loop_last = last;
    }
    
    SYS_ARCH_UNPROTECT(lev);

    tcpip_callback_with_block((tcpip_callback_fn )netif_poll, netif, 0);
}

void netif_poll(struct netif *netif)
{
	SYS_ARCH_DECL_PROTECT(lev);

	SYS_ARCH_PROTECT(lev);
	while(netif->loop_first != NULL)
	{
		struct pbuf *in, in_end;
		in = in_end = netif->loop_first;
		while(in_end->len != in_end->tot_len)
		{
			in_end = in_end->next;
		}

		if(in_end == netif->loop_last)
		{
			netif->loop_first = netif->loop_first = NULL;
		}
		else
		{
			netif->loop_first = in_end->next;
		}
		in_end->next = NULL;
		SYS_ARCH_UNPROTECT(lev);
		//this don't need protect
		err_t err;
		err = ip_input(in, netif);
		if(err)
		{
			pbuf_free(in);
		}
		SYS_ARCH_PROTECT(lev);
	}
	SYS_ARCH_UNPROTECT(lev);
}
static err_t netif_loop_output_ipv4(struct netif *netif, struct pbuf *p, ip4_addr_t *addr)
{
    return netif_loop_output(netif, p);
}


static err_t netif_loopif_init(struct netif *netif)
{
    netif->name[0] = 'l';
    netif->name[1] = 'o';
    netif->output = netif_loop_output_ipv4;
    return ERR_OK;
}
void netif_init()
{
    ip4_addr_t loop_ipaddr, loop_netmask, loop_gw;
    IP4_ADDR(&loop_ipaddr, 127,0,0,1);
    IP4_ADDR(&loop_netmask, 255,0,0,0);
    IP4_ADDR(&loop_gw, 127,0,0,1);

    netif_add(&loop_netif, &loop_ipaddr, &loop_netmask,
        &loop_gw, NULL, netif_loopif_init, tcpip_init);

    netif_set_link_up(&loop_netif);
    netif_set_up(&loop_netif);
}

