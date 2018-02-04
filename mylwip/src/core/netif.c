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

//#include "netif/ethernet.h"

struct netif *netif_list;
struct netif *netif_default;



struct netif *
netif_add(struct netif *netif,
          const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
          void *state, netif_init_fn init, netif_input_fn input)
{
    return NULL;
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

