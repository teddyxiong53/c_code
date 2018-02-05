#include "lwip/opt.h"

#include "lwip/opt.h"
#include "lwip/debug.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/sys.h"
#include "lwip/netif.h"
#include "lwip/stats.h"
#include "lwip/tcpip.h"
//#include "lwip/dhcp.h"
//#include "lwip/netifapi.h"

//#include "netif/etharp.h"
#include "netif/ethernetif.h"

#include "lwip/inet.h"




void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
}

void list_if(void)
{
    struct netif *netif;
    rt_enter_critical();
    int i =0;
    netif = netif_list;
    while(netif != NULL)
    {
        rt_kprintf("network interface: %c%c%s \n",
            netif->name[0], netif->name[1],
            (netif == netif_default)?"(Default)":"");
        rt_kprintf("MTU: %d \n", netif->mtu);
        rt_kprintf("MAC: ");
        for(i=0; i<netif->hwaddr_len; i++)
        {
            rt_kprintf("%02x ", netif->hwaddr[i]);
        }
        rt_kprintf("\nFLAGS: ");
        if(netif->flags & NETIF_FLAG_UP)
        {
            rt_kprintf("UP");
        }
        else 
        {
            rt_kprintf("DOWN");
        }
        if(netif->flags & NETIF_FLAG_LINK_UP)
        {
            rt_kprintf("LINK UP");
        }
        else 
        {
            rt_kprintf("LINK DOWN");
        }
        rt_kprintf("\n");
        rt_kprintf("ip address: %s \n", ipaddr_ntoa(&(netif->ip_addr)));
        rt_kprintf("netmask: %s \n", ipaddr_ntoa(&(netif->netmask)));
        rt_kprintf("gw: %s \n", ipaddr_ntoa(&(netif->gw)));
        rt_kprintf("\n");
        netif = netif->next;
    }
    rt_exit_critical();
}

int eth_system_device_init_private(void)
{
}

#if LWIP_DNS
#include <lwip/dns.h>
void set_dns(char* dns_server)
{

}
//FINSH_FUNCTION_EXPORT(set_dns, set DNS server address);
#endif





