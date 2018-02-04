
#ifndef __ARCH_SYS_ARCH_H__
#define __ARCH_SYS_ARCH_H__

#include "arch/cc.h"

#include <rtthread.h>

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#define SYS_MBOX_NULL RT_NULL
#define SYS_SEM_NULL  RT_NULL

typedef u32_t sys_prot_t;

#define SYS_MBOX_SIZE 10
#define SYS_LWIP_TIMER_NAME "timer"
#define SYS_LWIP_MBOX_NAME "mbox"
#define SYS_LWIP_SEM_NAME "sem"
#define SYS_LWIP_MUTEX_NAME "mu"

typedef rt_sem_t sys_sem_t;
typedef rt_mutex_t sys_mutex_t;
typedef rt_mailbox_t  sys_mbox_t;
typedef rt_thread_t sys_thread_t;

#endif /* __ARCH_SYS_ARCH_H__ */
