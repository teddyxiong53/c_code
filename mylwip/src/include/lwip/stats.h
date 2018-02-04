
#ifndef LWIP_HDR_STATS_H
#define LWIP_HDR_STATS_H

#include "lwip/opt.h"

#include "lwip/mem.h"
#include "lwip/memp.h"

#ifdef __cplusplus
extern "C" {
#endif



#define stats_init()
#define STATS_INC(x)
#define STATS_DEC(x)
#define STATS_INC_USED(x)



#define TCP_STATS_INC(x)
#define TCP_STATS_DISPLAY()



#define UDP_STATS_INC(x)
#define UDP_STATS_DISPLAY()



#define ICMP_STATS_INC(x)
#define ICMP_STATS_DISPLAY()



#define IGMP_STATS_INC(x)
#define IGMP_STATS_DISPLAY()



#define IP_STATS_INC(x)
#define IP_STATS_DISPLAY()


#define IPFRAG_STATS_INC(x)
#define IPFRAG_STATS_DISPLAY()



#define ETHARP_STATS_INC(x)
#define ETHARP_STATS_DISPLAY()



#define LINK_STATS_INC(x)
#define LINK_STATS_DISPLAY()


#define MEM_STATS_AVAIL(x, y)
#define MEM_STATS_INC(x)
#define MEM_STATS_INC_USED(x, y)
#define MEM_STATS_DEC_USED(x, y)
#define MEM_STATS_DISPLAY()



#define MEMP_STATS_DEC(x, i)
#define MEMP_STATS_DISPLAY(i)
#define MEMP_STATS_GET(x, i) 0



#define SYS_STATS_INC(x)
#define SYS_STATS_DEC(x)
#define SYS_STATS_INC_USED(x)
#define SYS_STATS_DISPLAY()




#define stats_display()
#define stats_display_proto(proto, name)
#define stats_display_igmp(igmp, name)
#define stats_display_mem(mem, name)
#define stats_display_memp(mem, index)
#define stats_display_sys(sys)


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_STATS_H */
