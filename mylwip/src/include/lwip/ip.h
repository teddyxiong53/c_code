#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/netif.h"
#include "lwip/ip4.h"
//#include "lwip/ip6.h"
#include "lwip/prot/ip.h"


struct ip_pcb
{
    ip_addr_t local_ip;
    ip_addr_t remote_ip;
    u8_t so_options; 
    u8_t tos;
    u8_t ttl;
    
};

struct ip_globals
{
    struct netif *current_netif;
    struct netif *current_input_netif;
    struct ip_hdr *current_ip4_header;
    u16_t current_ip_header_tot_len;
    ip_addr_t current_iphdr_src;
    ip_addr_t current_iphdr_dest;
};

extern struct ip_globals ip_data;

#define ip_current_netif()  (ip_data.current_netif)
#define ip_current_input_netif() (ip_data.current_input_netif)


#define ip_route(src, dest) \
    ip4_route_src(dest, src)

#define LWIP_IP_HDRINCL NULL

err_t ip_input(struct pbuf *p, struct netif *netif);

