#include "netif/ethernetif.h"




void set_if(char* netif_name, char* ip_addr, char* gw_addr, char* nm_addr)
{
}

void list_if(void)
{
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





