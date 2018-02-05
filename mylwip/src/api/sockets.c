#include "lwip/sockets.h"

#define NUM_SOCKETS MEMP_NUM_NETCONN 

#define set_errno(err) do {if(err) errno = (err);} while(0);
#define SOCKADDR4_TO_IP4ADDR_PORT(sin, ipaddr, port) \
                do {\
                    inet_addr_to_ip4addr(ip_2_ip4(ipaddr), &((sin)->sin_addr));\
                    port = lwip_ntohs((sin)->sin_port);\
                } while(0);
#define SOCKADDR_TO_IPADDR_PORT(sockaddr, ipaddr,port) \
    SOCKADDR4_TO_IP4ADDR_PORT((const struct sockaddr_in *)sockaddr, ipaddr, port)
#define SELWAIT_T u8_t



struct lwip_sock
{
    struct netconn *conn;
    void *lastdata;
    u16_t lastoffset;
    s16_t rcvevent;
    u16_t sendevent;
    u16_t errevent;
    u8_t err;
    SELWAIT_T select_waiting;
    rt_wqueue_t wait_head;
};

#define sock_set_errno(sk, e) do {\
								int sockerr = (e);\
								sk->err = (u8_t)sockerr;\
								set_errno(sockerr);\
								}while(0);





static struct lwip_sock sockets[NUM_SOCKETS];

int lwip_accept(int s, struct sockaddr *addr, socklen_t *addrlen)
{
}

int lwip_bind(int s, const struct sockaddr *name, socklen_t namelen)
{
}

int lwip_shutdown(int s, int how)
{
}

int lwip_getpeername (int s, struct sockaddr *name, socklen_t *namelen)
{
}

int lwip_getsockname (int s, struct sockaddr *name, socklen_t *namelen)
{
}

int lwip_getsockopt (int s, int level, int optname, void *optval, socklen_t *optlen)
{
}

int lwip_setsockopt (int s, int level, int optname, const void *optval, socklen_t optlen)
{
}

int lwip_close(int s)
{
}

int lwip_connect(int s, const struct sockaddr *name, socklen_t namelen)
{
}

int lwip_listen(int s, int backlog)
{
}

int lwip_recv(int s, void *mem, size_t len, int flags)
{
}

int lwip_read(int s, void *mem, size_t len)
{
}

int lwip_recvfrom(int s, void *mem, size_t len, int flags,
      struct sockaddr *from, socklen_t *fromlen)
{
	struct lwip_sock *sock;
	u8_t done = 0;
	void *buf =NULL;
	int off = 0;
	err_t err;
	struct pbuf *p;
	u16_t buflen, copylen;
	sock = get_socket(s);

	do {
		if(sock->lastdata)
		{
			buf = sock->lastdata;
			
		}
		else
		{
			if(
				((flags & MSG_DONTWAIT)
					|| netconn_is_nonblocking(sock->conn))
				&& (sock->rcvevent <= 0)
			)
			{
				if(off > 0)
				{
					sock_set_errno(sock, 0);
					return off;
				}
				set_errno(EWOULDBLOCK);
				return -1;
			}
			if(0)//tcp
			{
				
			}
			else
			{
				err= netconn_recv(sock->conn, (struct netbuf **)&buf);
			}
			if(err)
			{
				if(off > 0)
				{
					//todo
				}
			}
			
		}

		if(0)//tcp
		{
			//todo
		}
		else
		{
			p = ((struct netbuf *)buf)->p;
			
		}
		buflen = p->tot_len;
		buflen -= sock->lastoffset;
		if(len > buflen)
		{
			copylen = buflen;
		}
		else
		{
			copylen = (u16_t)len;
		}
		pbuf_copy_partial(p, (u8_t *)mem + off, copylen, sock->lastoffset);
		off += copylen;
		if(0)//tcp
		{
			
		}
		else
		{
			done =1;
		}

		if(done)
		{
			if(from && fromlen)
			{
				
			}
		}
	}while(!done);
	sock_set_errno(sock ,0);
	return off;
}

int lwip_send(int s, const void *dataptr, size_t size, int flags)
{
}

int lwip_sendmsg(int s, const struct msghdr *message, int flags)
{
	
}

struct lwip_sock * get_socket(int s)
{
    struct lwip_sock *sock;
    s -= LWIP_SOCKET_OFFSET;
    if(
        (s < 0)
        || (s > NUM_SOCKETS)
        )
    {
        myloge("int s out of range:%d ", s);
        set_errno(EBADF);
        return NULL;
    }
    sock = sockets[s];
    if(sock->conn != NULL)//this means is used
    {
        myloge("the sock is used now");
        set_errno(EBADF);
        return NULL;
    }
    return sock;
}
int lwip_sendto(int s, const void *data, size_t size, int flags,
    const struct sockaddr *to, socklen_t tolen)
{
    struct lwip_sock *sock;
    u16_t short_size;
    struct netbuf buf;
    u16_t remote_port;
    err_t err;
    sock = get_socket(s);
    if(sock == NULL)
    {
        myloge("can't get sock");
        return -1;
    }
    short_size = (u16_t)size;
    buf.p = buf.ptr = NULL;
    if(to != NULL)
    {
        SOCKADDR_TO_IPADDR_PORT(to, &buf.addr, remote_port);
    }
    else
    {
        remote_port = 0;
        ip_addr_set_any(0, &buf.addr);
    }
    netbuf_fromport(&buf) = remote_port;
    err = netbuf_ref(&buf, data, short_size);
    if(err == ERR_OK)
    {
        err = netconn_send(sock->conn, &buf);
    }
    netbuf_free(&buf);
}

static void event_callback
(
    struct netconn *conn,
    enum netconn_evt evt,
    u16_t len
)
{
    int s;
    if(conn)
    {
        s = conn->socket;
        
    }
}

static int alloc_socket(struct netconn *conn, int accept)
{
    int i;
    SYS_ARCH_DECL_PROTECT(lev);
    for(i=0; i<NUM_SOCKETS; i++)
    {
        SYS_ARCH_PROTECT(lev);//disable sched
        if((sockets[i].conn == NULL)
                && (sockets[i].select_waiting == 0)
                )
        {
            sockets[i].conn =conn;
            sockets[i].lastdata = NULL;
            sockets[i].lastoffset = 0;
            sockets[i].rcvevent = 0;
            sockets[i].sendevent = (NETCONNTYPE_GROUP(conn->type)==NETCONN_TCP?(accept!=0):1);
            sockets[i].errevent = 0;
            sockets[i].err = 0;
            return i + LWIP_SOCKET_OFFSET;
            
        }
        SYS_ARCH_UNPROTECT(lev);
    }
    return -1;
}
int lwip_socket(int domain, int type, int protocol)
{
    struct netconn *conn;
    int i;

    switch(type)
    {
        case SOCK_RAW:
            conn = netconn_new_with_proto_and_callback(NETCONN_RAW, protocol, event_callback);
    }
    if(!conn)
    {
        myloge("socket failed");
        set_errno(ENOBUFS);
        return -1;
    }
    i = alloc_socket(conn, 0);
    if(i < 0)
    {
        myloge("alloc socket failed ");
        netconn_delete(conn);
        set_errno(ENFILE);
        return -1;
    }
    conn->socket = i;
    mylogd("new socket ok");
    set_errno(0);
    return i;
    
}

int lwip_write(int s, const void *dataptr, size_t size)
{
}

int lwip_writev(int s, const struct iovec *iov, int iovcnt)
{
}

int lwip_select(int maxfdp1, fd_set *readset, fd_set *writeset, fd_set *exceptset,
                struct timeval *timeout)
{
}

int lwip_ioctl(int s, long cmd, void *argp)
{
}

int lwip_fcntl(int s, int cmd, int val)
{
}


struct lwip_sock *lwip_tryget_socket(int s)
{
	return NULL;
}

