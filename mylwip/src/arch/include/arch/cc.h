
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <rthw.h>
#include <rtthread.h>

#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"



#include <errno.h>
/* some errno not defined in newlib */
#define ENSRNOTFOUND 163  /* Domain name not found */
/* WARNING: ESHUTDOWN also not defined in newlib. We chose
			180 here because the number "108" which is used
			in arch.h has been assigned to another error code. */
#define ESHUTDOWN 180


#include <sys/time.h>
#define LWIP_TIMEVAL_PRIVATE	   0



#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END

void sys_arch_assert(const char* file, int line);
#define LWIP_PLATFORM_DIAG(x)	do {rt_kprintf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x) do {rt_kprintf(x); sys_arch_assert(__FILE__, __LINE__);}while(0)

#include "string.h"

#define SYS_ARCH_DECL_PROTECT(level)	
#define SYS_ARCH_PROTECT(level)		rt_enter_critical()
#define SYS_ARCH_UNPROTECT(level) 	rt_exit_critical()

#endif /* __ARCH_CC_H__ */

