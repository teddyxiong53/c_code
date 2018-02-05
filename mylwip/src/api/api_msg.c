#include "lwip/opt.h"


#include "lwip/priv/api_msg.h"

#include "lwip/ip.h"
#include "lwip/ip_addr.h"
//#include "lwip/udp.h"
//#include "lwip/tcp.h"
#include "lwip/raw.h"

#include "lwip/memp.h"
#include "lwip/dns.h"

#include "lwip/priv/tcpip_priv.h"

#include <string.h>

#define TCPIP_APIMSG_ACK(m) NETCONN_SET_SAFE_ERR((m)->con, (m)->err)


struct netconn *netconn_alloc
(
    enum netconn_type t,
    netconn_callback callback
)
{
    struct netconn *conn;
    int size;
    conn = (struct netconn *)memp_malloc(MEMP_NETCONN);
    if(conn == NULL)
    {
        myloge("memp_malloc netconn failed");
        return NULL;
    }
    conn->last_err = ERR_OK;
    conn->type = t;
    conn->pcb.tcp = NULL;
    switch(NETCONNTYPE_GROUP(t))
    {
        case NETCONN_RAW:
            size = DEFAULT_RAW_RECVMBOX_SIZE;// 1
            break;
        default:
            myloge("wrong netconn type");
            //break;
            goto free_and_return;
    }
    if(sys_mbox_new(&conn->recvmbox, size) != ERR_OK)
    {
        myloge("sys_mbox_new failed");
        goto free_and_return;
    }
    if(sys_sem_new(&conn->op_completed, 0) != ERR_OK)
    {
        sys_mbox_free(&conn->recvbox);
        myloge("sys_sem_new failed");
        goto free_and_return;
    }
    conn->state = NETCONN_NONE;
    conn->socket = -1;
    conn->callback = callback;

    conn->send_timeout = 0;
    conn->recv_timeout = 0;

    conn->recv_bufsize = RECV_BUFSIZE_DEFAULT;
    conn->recv_avail = 0;

    conn->flags = 0;
    
    return conn;
free_and_return:
    memp_free(MEMP_NETCONN, conn);
}


static u8_t recv_raw
(
    void *arg,
    struct raw_pcb *pcb, 
    struct pbuf *p,
    const ip_addr_t *addr
)
{
    struct pbuf *q;
    struct netconn *conn;
    struct netbuf *buf;
    conn = (struct netconn *)arg;
    if(
        (conn != NULL)
        && (sys_mbox_valid(&conn->recvmbox))
        )
    {
        int recv_avail;
        //safe get 
        SYS_ARCH_GET(conn->recv_avail, recv_avail);
        if((recv_avail + (int)p->tot_len) > conn->recv_bufsize)
        {
            mylogw("buf overflow");
            return 0;
        }
        q = pbuf_alloc(PBUF_RAW, p->tot_len, PBUF_RAM);
        if(q != NULL)
        {
            if(pbuf_copy(q, p) != 0)
            {
                pbuf_free(q);
                q = NULL;
            }
        }
        if(q != NULL)
        {
            u16_t len;
            buf = (struct netbuf *)memp_malloc(MEMP_NETBUF);
            if(buf == NULL)
            {
                myloge("memp malloc failed ");
                pbuf_free(q);
                return 0;
            }
            // buf != null
            buf->p = q;
            buf->ptr = q;
            ip_addr_copy(buf->addr, *ip_data.current_iphdr_src);
            buf->port = pcb->protocol;
            len = q->tot_len;
            if(sys_mbox_trypost(&conn->recvmbox, buf) != 0)
            {
                netbuf_delete(buf);
                return 0;
            }
            else 
            {
                SYS_ARCH_INC(conn->recv_avail, len);
                if(conn->callback != NULL)
                {
                    conn->callback(conn, NETCONN_EVT_RCVPLUS, len);
                }
            }
        }
        
    }
}
static void pcb_new(struct api_msg *msg)
{
    switch(NETCONNTYPE_GROUP(msg->conn->type))
    {
        case NETCONN_RAW:
            msg->conn->pcb.raw = raw_new(msg->msg.n.proto);
            if(msg->conn->pcb.raw != NULL)
            {
                raw_recv(msg->conn->pcb.raw, recv_raw, msg->conn);
            }
            break;
        default:
            break;
    }
}

void lwip_netconn_do_newconn(void *m)
{
    struct api_msg *msg = (struct api_msg *)m;
    msg->err = ERR_OK;
    if(msg->conn->pcb.tcp == NULL)
    {
        pcb_new(msg);
    }
    TCPIP_APIMSG_ACK(msg);
}


err_t lwip_netconn_do_send(void *m)
{
    struct api_msg *msg = (struct api_msg *)m;
    if(ERR_IS_FATAL(msg->conn->last_err))
    {
        msg->err = msg->conn->last_err;
    }
    else
    {
        msg->err = ERR_CONN;
        if(msg->conn->pcb.tcp != NULL)
        {
            if(ip_addr_isany(&msg->msg.b->addr)
                    || (IP_IS_ANY_TYPE_VAL(msg->msg.b->addr)))
            {
                msg->err = raw_send();
            }
            else
            {
                msg->err = raw_sendto();
            }
        }
    }
}
