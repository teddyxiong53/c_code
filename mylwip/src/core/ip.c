#include "lwip/opt.h"


#include "lwip/ip_addr.h"
#include "lwip/ip.h"


struct ip_globals ip_data;


err_t ip_input(struct pbuf *p, struct netif *netif)
{
	if(p)
	{
		return ip4_input(p, netif);
	}
	return ERR_VAL;
}