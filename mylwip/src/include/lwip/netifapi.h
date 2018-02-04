
#ifndef LWIP_HDR_NETIFAPI_H
#define LWIP_HDR_NETIFAPI_H

#include "lwip/opt.h"


#include "lwip/sys.h"
#include "lwip/netif.h"

#include "lwip/priv/tcpip_priv.h"

#ifdef __cplusplus
extern "C" {
#endif


#define NETIFAPI_IPADDR_DEF(type, m)  const type * m


typedef void (*netifapi_void_fn)(struct netif *netif);
typedef err_t (*netifapi_errt_fn)(struct netif *netif);

struct netifapi_msg {
  struct tcpip_api_call_data call;
  struct netif *netif;
  union {
    struct {
      ip4_addr_t *ipaddr;
      ip4_addr_t *netmask;
      ip4_addr_t *gw;
      void *state;
      netif_init_fn init;
      netif_input_fn input;
    } add;
    struct {
      netifapi_void_fn voidfunc;
      netifapi_errt_fn errtfunc;
    } common;
  } msg;
};



err_t netifapi_netif_add(struct netif *netif,
                         const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,
                         void *state, netif_init_fn init, netif_input_fn input);


err_t netifapi_netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr,
                              const ip4_addr_t *netmask, const ip4_addr_t *gw);


err_t netifapi_netif_common(struct netif *netif, netifapi_void_fn voidfunc,
                            netifapi_errt_fn errtfunc);

/** @ingroup netifapi_netif */
#define netifapi_netif_remove(n)        netifapi_netif_common(n, netif_remove, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_up(n)        netifapi_netif_common(n, netif_set_up, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_down(n)      netifapi_netif_common(n, netif_set_down, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_default(n)   netifapi_netif_common(n, netif_set_default, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_up(n)   netifapi_netif_common(n, netif_set_link_up, NULL)
/** @ingroup netifapi_netif */
#define netifapi_netif_set_link_down(n) netifapi_netif_common(n, netif_set_link_down, NULL)


#ifdef __cplusplus
}
#endif


#endif /* LWIP_HDR_NETIFAPI_H */
