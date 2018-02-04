
#ifndef LWIP_HDR_TIMEOUTS_H
#define LWIP_HDR_TIMEOUTS_H

#include "lwip/opt.h"
#include "lwip/err.h"

#include "lwip/sys.h"


#ifdef __cplusplus
extern "C" {
#endif


#define LWIP_DEBUG_TIMERNAMES SYS_DEBUG


typedef void (* lwip_cyclic_timer_handler)(void);

struct lwip_cyclic_timer {
  u32_t interval_ms;
  lwip_cyclic_timer_handler handler;
#if LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
#endif /* LWIP_DEBUG_TIMERNAMES */
};


extern const struct lwip_cyclic_timer lwip_cyclic_timers[];



typedef void (* sys_timeout_handler)(void *arg);

struct sys_timeo {
  struct sys_timeo *next;
  u32_t time;
  sys_timeout_handler h;
  void *arg;
#if LWIP_DEBUG_TIMERNAMES
  const char* handler_name;
#endif /* LWIP_DEBUG_TIMERNAMES */
};

void sys_timeouts_init(void);

#if LWIP_DEBUG_TIMERNAMES
void sys_timeout_debug(u32_t msecs, sys_timeout_handler handler, void *arg, const char* handler_name);
#define sys_timeout(msecs, handler, arg) sys_timeout_debug(msecs, handler, arg, #handler)
#else /* LWIP_DEBUG_TIMERNAMES */
void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg);
#endif /* LWIP_DEBUG_TIMERNAMES */

void sys_untimeout(sys_timeout_handler handler, void *arg);
void sys_restart_timeouts(void);

void sys_timeouts_mbox_fetch(sys_mbox_t *mbox, void **msg);





#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_TIMEOUTS_H */
