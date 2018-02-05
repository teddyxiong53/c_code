#include "lwip/opt.h"

#include "lwip/priv/tcpip_priv.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/mem.h"
#include "lwip/init.h"
//#include "lwip/ip.h"
#include "lwip/pbuf.h"
//#include "lwip/etharp.h"
//#include "netif/ethernet.h"



static tcpip_init_done_fn tcpip_init_done;
static void * tcpip_init_done_arg;
static sys_mbox_t mbox;

sys_mutex_t lock_tcpip_core;


#if 1
static void tcpip_thread(void *arg)
{
    struct tcpip_msg *msg;
    if(tcpip_init_done != NULL)
    {
        tcpip_init_done(tcpip_init_done_arg);
    }
    
    rt_kprintf("lwip[debug] -- tcpip init done \n");

    #if 0
    LOCK_TCPIP_CORE();
    
    while(1)
    {
        UNLOCK_TCPIP_CORE();
        sys_timeouts_mbox_fetch(&mbox, (void **)&msg);
        LOCK_TCPIP_CORE();
        if(msg == NULL)
        {
            //rt_kprintf("lwip[warn] -- tcpip_thread invalid msg \n");
            continue;
        }
        switch(msg->type)
        {
            default:
                rt_kprintf("lwip[error] -- wrong msg type\n");
                break;
        }
    }
    #endif
}
err_t  tcpip_input(struct pbuf *p, struct netif *inp)
{
    return 0;
}

err_t tcpip_callback_with_block
(
    tcpip_callback_fn func,
    void *ctx,
    u8_t block
)
{
    struct tcpip_msg *msg;
    msg = (struct tcpip_msg *)memp_malloc(MEMP_TCPIP_MSG_API);
    if(msg == NULL)
    {
        myloge("memp_alloc failed");
        return ERR_MEM;
    }
    msg->type = TCPIP_MSG_CALLBACK;
    msg->msg.cb.function = func;
    msg->msg.cb.ctx = ctx;
    if(block)
    {
        sys_mbox_post(&mbox, msg);
    }
    else
    {
        if((sys_mbox_trypost(&mbox, msg)) != ERR_OK)
        {
            memp_free(MEMP_TCPIP_MSG_API, msg);
            myloge("sys_mbox_trypost failed");
            return ERR_MEM;
        }
    }
    return ERR_OK;
}
void tcpip_init(tcpip_init_done_fn initfunc, void *arg)
{
    err_t ret;
    lwip_init();

    
    tcpip_init_done = initfunc;
    tcpip_init_done_arg = arg;
    ret = sys_mbox_new(&mbox, TCPIP_MBOX_SIZE);
    if(ret < 0)
    {
        rt_kprintf("lwip -- failed to create mbox\n");
        return;
    }
    ret = sys_mutex_new(&lock_tcpip_core);
    if(ret < 0)
    {
        rt_kprintf("lwip -- failed to create lock_tcpip_core \n");
        return;
    }
    sys_thread_new("tcpip", tcpip_thread, NULL, 1024, 8);
    
}

#endif
