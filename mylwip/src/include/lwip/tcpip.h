
#ifndef LWIP_HDR_TCPIP_H
#define LWIP_HDR_TCPIP_H

#include "lwip/opt.h"


#include "lwip/err.h"
#include "lwip/timeouts.h"
#include "lwip/netif.h"

#ifdef __cplusplus
extern "C" {
#endif


extern sys_mutex_t lock_tcpip_core;
#define LOCK_TCPIP_CORE()     sys_mutex_lock(&lock_tcpip_core)
#define UNLOCK_TCPIP_CORE()   sys_mutex_unlock(&lock_tcpip_core)


struct pbuf;
struct netif;


typedef void (*tcpip_init_done_fn)(void *arg);
typedef void (*tcpip_callback_fn)(void *ctx);


struct tcpip_callback_msg;

void   tcpip_init(tcpip_init_done_fn tcpip_init_done, void *arg);

err_t  tcpip_inpkt(struct pbuf *p, struct netif *inp, netif_input_fn input_fn);
err_t  tcpip_input(struct pbuf *p, struct netif *inp);

err_t  tcpip_callback_with_block(tcpip_callback_fn function, void *ctx, u8_t block);

#define tcpip_callback(f, ctx)  tcpip_callback_with_block(f, ctx, 1)

struct tcpip_callback_msg* tcpip_callbackmsg_new(tcpip_callback_fn function, void *ctx);
void   tcpip_callbackmsg_delete(struct tcpip_callback_msg* msg);
err_t  tcpip_trycallback(struct tcpip_callback_msg* msg);

err_t  pbuf_free_callback(struct pbuf *p);
err_t  mem_free_callback(void *m);

#ifdef __cplusplus
}
#endif



#endif /* LWIP_HDR_TCPIP_H */
