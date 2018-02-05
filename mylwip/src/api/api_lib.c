#include "lwip/opt.h"


#include "lwip/api.h"
#include "lwip/memp.h"

#include "lwip/ip.h"
#include "lwip/raw.h"
//#include "lwip/udp.h"
#include "lwip/priv/api_msg.h"
//#include "lwip/priv/tcp_priv.h"
#include "lwip/priv/tcpip_priv.h"

#include <string.h>

static err_t netconn_apimsg
(
    tcpip_callback_fn fn, 
    struct api_msg *apimsg
)
{
    err_t err;
    err = tcpip_send_msg_wait_sem(fn, apimsg, LWIP_API_MSG_SEM(apimsg));
    return err;
}

struct netconn * netconn_new_with_proto_and_callback
(
    enum netconn_type t,
    u8_t proto, 
    netconn_callback callback
)
{
    struct netconn *conn;
    struct api_msg msg;
    conn = netconn_alloc(t, callback);
    if(conn != NULL)
    {
        err_t err;
        msg.msg.n.proto = proto;
        msg.conn = conn;
        err = netconn_apimsg(lwip_netconn_do_newconn, &msg);
        if(err != 0) //if error 
        {
            sys_sem_free(&conn->op_completed);
            sys_mbox_free(&conn->recvmbox);
            memp_free(MEMP_NETCONN, conn);
        }
    }
    return NULL;
}


err_t netconn_send(struct netconn *conn, struct netbuf *buf)
{
    struct api_msg msg;
    err_t err;
    msg.conn = conn;
    msg.msg.b =buf;
    err = netconn_apimsg(lwip_netconn_do_send, &msg);
    return err;
}

static netconn_recv_data
(
	struct netconn *conn,
	void **new_buf
)
{
	void *buf = NULL;
	u16_t len;
	*new_buf = NULL;
	if(0)//error is fatal
	{
		return conn->last_err;
	}
	if(sys_arch_mbox_fetch(&conn->recvmbox, &buf, conn->recv_timeout)
		== SYS_ARCH_TIMEOUT)
	{
		return ERR_TIMEOUT;
	}
	len = netbuf_len((struct netbuf *)buf);
	SYS_ARCH_DEC(conn->recv_avail, len);
	if(conn->callback)
	{
		conn->callback(conn, NETCONN_EVT_RCVMINUS, len);
	}
	*new_buf = buf;
	return ERR_OK;
}
err_t netconn_recv
(
	struct netconn *conn, 
	struct netbuf **newbuf
)
{
	return netconn_recv_data(conn, (void **)newbuf);
}