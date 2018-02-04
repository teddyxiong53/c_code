
#ifndef LWIP_HDR_NETIF_H
#define LWIP_HDR_NETIF_H

#include "lwip/opt.h"

#define ENABLE_LOOPBACK (LWIP_NETIF_LOOPBACK || LWIP_HAVE_LOOPIF)

#include "lwip/err.h"

#include "lwip/ip_addr.h"

#include "lwip/def.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"

#ifdef __cplusplus
extern "C" {
#endif


#ifndef NETIF_MAX_HWADDR_LEN
#define NETIF_MAX_HWADDR_LEN 6U
#endif


#define NETIF_FLAG_UP           0x01U
#define NETIF_FLAG_BROADCAST    0x02U
#define NETIF_FLAG_LINK_UP      0x04U
#define NETIF_FLAG_ETHARP       0x08U
#define NETIF_FLAG_ETHERNET     0x10U
#define NETIF_FLAG_IGMP         0x20U
#define NETIF_FLAG_MLD6         0x40U







struct netif;


enum netif_mac_filter_action {
  NETIF_DEL_MAC_FILTER = 0,
  NETIF_ADD_MAC_FILTER = 1
};

typedef err_t (*netif_init_fn)(struct netif *netif);
typedef err_t (*netif_input_fn)(struct pbuf *p, struct netif *inp);


typedef err_t (*netif_output_fn)(struct netif *netif, struct pbuf *p,
       const ip4_addr_t *ipaddr);




typedef err_t (*netif_linkoutput_fn)(struct netif *netif, struct pbuf *p);
typedef void (*netif_status_callback_fn)(struct netif *netif);



struct netif {
  struct netif *next;
  ip_addr_t ip_addr;
  ip_addr_t netmask;
  ip_addr_t gw;


  netif_input_fn input;
  netif_output_fn output;


  netif_linkoutput_fn linkoutput;

  netif_status_callback_fn status_callback;


  void *state;

  u16_t mtu;

  u8_t hwaddr_len;
  u8_t hwaddr[NETIF_MAX_HWADDR_LEN];
  u8_t flags;
  char name[2];
  u8_t num;


  struct pbuf *loop_first;
  struct pbuf *loop_last;


};


#define NETIF_SET_CHECKSUM_CTRL(netif, chksumflags)
#define IF__NETIF_CHECKSUM_ENABLED(netif, chksumflag)



extern struct netif *netif_list;

extern struct netif *netif_default;

void netif_init(void);

struct netif *netif_add(struct netif *netif,

                        const ip4_addr_t *ipaddr, const ip4_addr_t *netmask, const ip4_addr_t *gw,

                        void *state, netif_init_fn init, netif_input_fn input);

void netif_set_addr(struct netif *netif, const ip4_addr_t *ipaddr, const ip4_addr_t *netmask,
                    const ip4_addr_t *gw);

void netif_remove(struct netif * netif);


struct netif *netif_find(const char *name);

void netif_set_default(struct netif *netif);


void netif_set_ipaddr(struct netif *netif, const ip4_addr_t *ipaddr);
void netif_set_netmask(struct netif *netif, const ip4_addr_t *netmask);
void netif_set_gw(struct netif *netif, const ip4_addr_t *gw);

#define netif_ip4_addr(netif)    ((const ip4_addr_t*)ip_2_ip4(&((netif)->ip_addr)))

#define netif_ip4_netmask(netif) ((const ip4_addr_t*)ip_2_ip4(&((netif)->netmask)))

#define netif_ip4_gw(netif)      ((const ip4_addr_t*)ip_2_ip4(&((netif)->gw)))

#define netif_ip_addr4(netif)    ((const ip_addr_t*)&((netif)->ip_addr))

#define netif_ip_netmask4(netif) ((const ip_addr_t*)&((netif)->netmask))

#define netif_ip_gw4(netif)      ((const ip_addr_t*)&((netif)->gw))


void netif_set_up(struct netif *netif);
void netif_set_down(struct netif *netif);

#define netif_is_up(netif) (((netif)->flags & NETIF_FLAG_UP) ? (u8_t)1 : (u8_t)0)



void netif_set_status_callback(struct netif *netif, netif_status_callback_fn status_callback);






void netif_set_link_up(struct netif *netif);
void netif_set_link_down(struct netif *netif);

#define netif_is_link_up(netif) (((netif)->flags & NETIF_FLAG_LINK_UP) ? (u8_t)1 : (u8_t)0)




err_t netif_loop_output(struct netif *netif, struct pbuf *p);
void netif_poll(struct netif *netif);

void netif_poll_all(void);


err_t netif_input(struct pbuf *p, struct netif *inp);



#define NETIF_SET_HWADDRHINT(netif, hint)


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_NETIF_H */
