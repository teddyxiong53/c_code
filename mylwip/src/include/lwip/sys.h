
#ifndef LWIP_HDR_SYS_H
#define LWIP_HDR_SYS_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif



#define SYS_ARCH_TIMEOUT 0xffffffffUL


#define SYS_MBOX_EMPTY SYS_ARCH_TIMEOUT

#include "lwip/err.h"
#include "arch/sys_arch.h"


typedef void (*lwip_thread_fn)(void *arg);





err_t sys_mutex_new(sys_mutex_t *mutex);

void sys_mutex_lock(sys_mutex_t *mutex);

void sys_mutex_unlock(sys_mutex_t *mutex);

void sys_mutex_free(sys_mutex_t *mutex);

int sys_mutex_valid(sys_mutex_t *mutex);


void sys_mutex_set_invalid(sys_mutex_t *mutex);




err_t sys_sem_new(sys_sem_t *sem, u8_t count);

void sys_sem_signal(sys_sem_t *sem);

u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout);

void sys_sem_free(sys_sem_t *sem);

#define sys_sem_wait(sem)                  sys_arch_sem_wait(sem, 0)

int sys_sem_valid(sys_sem_t *sem);


void sys_sem_set_invalid(sys_sem_t *sem);


#define sys_sem_valid_val(sem)       sys_sem_valid(&(sem))


#define sys_sem_set_invalid_val(sem) sys_sem_set_invalid(&(sem))



void sys_msleep(u32_t ms); /* only has a (close to) 1 ms resolution. */



err_t sys_mbox_new(sys_mbox_t *mbox, int size);

void sys_mbox_post(sys_mbox_t *mbox, void *msg);

err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg);

u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout);


u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg);


#define sys_mbox_tryfetch(mbox, msg) sys_arch_mbox_tryfetch(mbox, msg)

void sys_mbox_free(sys_mbox_t *mbox);
#define sys_mbox_fetch(mbox, msg) sys_arch_mbox_fetch(mbox, msg, 0)

int sys_mbox_valid(sys_mbox_t *mbox);


void sys_mbox_set_invalid(sys_mbox_t *mbox);


#define sys_mbox_valid_val(mbox)       sys_mbox_valid(&(mbox))


#define sys_mbox_set_invalid_val(mbox) sys_mbox_set_invalid(&(mbox))



sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread, void *arg, int stacksize, int prio);




void sys_init(void);


u32_t sys_jiffies(void);



u32_t sys_now(void);





#ifndef SYS_ARCH_INC
#define SYS_ARCH_INC(var, val) do { \
                                SYS_ARCH_DECL_PROTECT(old_level); \
                                SYS_ARCH_PROTECT(old_level); \
                                var += val; \
                                SYS_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SYS_ARCH_INC */

#ifndef SYS_ARCH_DEC
#define SYS_ARCH_DEC(var, val) do { \
                                SYS_ARCH_DECL_PROTECT(old_level); \
                                SYS_ARCH_PROTECT(old_level); \
                                var -= val; \
                                SYS_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SYS_ARCH_DEC */

#ifndef SYS_ARCH_GET
#define SYS_ARCH_GET(var, ret) do { \
                                SYS_ARCH_DECL_PROTECT(old_level); \
                                SYS_ARCH_PROTECT(old_level); \
                                ret = var; \
                                SYS_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SYS_ARCH_GET */

#ifndef SYS_ARCH_SET
#define SYS_ARCH_SET(var, val) do { \
                                SYS_ARCH_DECL_PROTECT(old_level); \
                                SYS_ARCH_PROTECT(old_level); \
                                var = val; \
                                SYS_ARCH_UNPROTECT(old_level); \
                              } while(0)
#endif /* SYS_ARCH_SET */


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_SYS_H */
