

#include "lwip/opt.h"

#include "lwip/init.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/pbuf.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
//#include "lwip/ip.h"
//#include "lwip/raw.h"
//#include "lwip/udp.h"
//#include "lwip/priv/tcp_priv.h"
//#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/timeouts.h"
//#include "lwip/etharp.h"

#include "lwip/api.h"




void lwip_init(void)
{
    mem_init();
    memp_init();
    pbuf_init();
    netif_init();
}







