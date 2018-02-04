
#ifndef LWIP_HDR_TCPIP_PRIV_H
#define LWIP_HDR_TCPIP_PRIV_H

#include "lwip/opt.h"


#include "lwip/tcpip.h"
#include "lwip/sys.h"
#include "lwip/timeouts.h"

#ifdef __cplusplus
extern "C" {
#endif

struct pbuf;
struct netif;



err_t tcpip_send_msg_wait_sem(tcpip_callback_fn fn, void *apimsg, sys_sem_t* sem);

struct tcpip_api_call_data
{
  u8_t dummy; /* avoid empty struct :-( */
};
typedef err_t (*tcpip_api_call_fn)(struct tcpip_api_call_data* call);
err_t tcpip_api_call(tcpip_api_call_fn fn, struct tcpip_api_call_data *call);

enum tcpip_msg_type {
  TCPIP_MSG_API,
  TCPIP_MSG_API_CALL,
  TCPIP_MSG_INPKT,
#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
  TCPIP_MSG_TIMEOUT,
  TCPIP_MSG_UNTIMEOUT,
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */
  TCPIP_MSG_CALLBACK,
  TCPIP_MSG_CALLBACK_STATIC
};

struct tcpip_msg {
  enum tcpip_msg_type type;
  union {
    struct {
      tcpip_callback_fn function;
      void* msg;
    } api_msg;
    struct {
      tcpip_api_call_fn function;
      struct tcpip_api_call_data *arg;
      sys_sem_t *sem;
    } api_call;
    struct {
      struct pbuf *p;
      struct netif *netif;
      netif_input_fn input_fn;
    } inp;
    struct {
      tcpip_callback_fn function;
      void *ctx;
    } cb;
#if LWIP_TCPIP_TIMEOUT && LWIP_TIMERS
    struct {
      u32_t msecs;
      sys_timeout_handler h;
      void *arg;
    } tmo;
#endif /* LWIP_TCPIP_TIMEOUT && LWIP_TIMERS */
  } msg;
};

#ifdef __cplusplus
}
#endif



#endif /* LWIP_HDR_TCPIP_PRIV_H */
