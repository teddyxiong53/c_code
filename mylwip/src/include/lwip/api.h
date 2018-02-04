
#ifndef LWIP_HDR_API_H
#define LWIP_HDR_API_H

#include "lwip/opt.h"

#if LWIP_NETCONN || LWIP_SOCKET /* don't build if not configured for use in lwipopts.h */

#include "lwip/arch.h"
#include "lwip/netbuf.h"
#include "lwip/sys.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"

#ifdef __cplusplus
extern "C" {
#endif


#define NETCONN_NOFLAG    0x00
#define NETCONN_NOCOPY    0x00 /* Only for source code compatibility */
#define NETCONN_COPY      0x01
#define NETCONN_MORE      0x02
#define NETCONN_DONTBLOCK 0x04


#define NETCONN_FLAG_NON_BLOCKING             0x02
#define NETCONN_FLAG_IN_NONBLOCKING_CONNECT   0x04
#define NETCONN_FLAG_CHECK_WRITESPACE         0x10


#define NETCONNTYPE_GROUP(t)         ((t)&0xF0)
#define NETCONNTYPE_DATAGRAM(t)      ((t)&0xE0)
#define NETCONNTYPE_ISIPV6(t)        (0)
#define NETCONNTYPE_ISUDPLITE(t)     ((t) == NETCONN_UDPLITE)
#define NETCONNTYPE_ISUDPNOCHKSUM(t) ((t) == NETCONN_UDPNOCHKSUM)



enum netconn_type {
  NETCONN_INVALID     = 0,
  /** TCP IPv4 */
  NETCONN_TCP         = 0x10,
  /** UDP IPv4 */
  NETCONN_UDP         = 0x20,
  /** UDP IPv4 lite */
  NETCONN_UDPLITE     = 0x21,
  /** UDP IPv4 no checksum */
  NETCONN_UDPNOCHKSUM = 0x22,



  NETCONN_RAW         = 0x40
};

enum netconn_state {
  NETCONN_NONE,
  NETCONN_WRITE,
  NETCONN_LISTEN,
  NETCONN_CONNECT,
  NETCONN_CLOSE
};


enum netconn_evt {
  NETCONN_EVT_RCVPLUS,
  NETCONN_EVT_RCVMINUS,
  NETCONN_EVT_SENDPLUS,
  NETCONN_EVT_SENDMINUS,
  NETCONN_EVT_ERROR
};



#if LWIP_DNS
/* Used for netconn_gethostbyname_addrtype(), these should match the DNS_ADDRTYPE defines in dns.h */
#define NETCONN_DNS_DEFAULT   NETCONN_DNS_IPV4_IPV6
#define NETCONN_DNS_IPV4      0
#define NETCONN_DNS_IPV6      1
#define NETCONN_DNS_IPV4_IPV6 2 /* try to resolve IPv4 first, try IPv6 if IPv4 fails only */
#define NETCONN_DNS_IPV6_IPV4 3 /* try to resolve IPv6 first, try IPv4 if IPv6 fails only */
#endif /* LWIP_DNS */

/* forward-declare some structs to avoid to include their headers */
struct ip_pcb;
struct tcp_pcb;
struct udp_pcb;
struct raw_pcb;
struct netconn;
struct api_msg;

/** A callback prototype to inform about events for a netconn */
typedef void (* netconn_callback)(struct netconn *, enum netconn_evt, u16_t len);

/** A netconn descriptor */
struct netconn {
  enum netconn_type type;
  enum netconn_state state;
  union {
    struct ip_pcb  *ip;
    struct tcp_pcb *tcp;
    struct udp_pcb *udp;
    struct raw_pcb *raw;
  } pcb;

  err_t last_err;
  sys_sem_t op_completed;

  sys_mbox_t recvmbox;
#if LWIP_TCP
  sys_mbox_t acceptmbox;
#endif /* LWIP_TCP */
#if LWIP_SOCKET
  int socket;
#endif /* LWIP_SOCKET */
#if LWIP_SO_SNDTIMEO
  s32_t send_timeout;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVTIMEO
  int recv_timeout;
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
  int recv_bufsize;
  int recv_avail;
#endif /* LWIP_SO_RCVBUF */
#if LWIP_SO_LINGER
  s16_t linger;
#endif /* LWIP_SO_LINGER */
  u8_t flags;
#if LWIP_TCP
  size_t write_offset;
  struct api_msg *current_msg;
#endif /* LWIP_TCP */
  netconn_callback callback;
};

/** Register an Network connection event */
#define API_EVENT(c,e,l) if (c->callback) {         \
                           (*c->callback)(c, e, l); \
                         }

/** Set conn->last_err to err but don't overwrite fatal errors */
#define NETCONN_SET_SAFE_ERR(conn, err) do { if ((conn) != NULL) { \
  SYS_ARCH_DECL_PROTECT(netconn_set_safe_err_lev); \
  SYS_ARCH_PROTECT(netconn_set_safe_err_lev); \
  if (!ERR_IS_FATAL((conn)->last_err)) { \
    (conn)->last_err = err; \
  } \
  SYS_ARCH_UNPROTECT(netconn_set_safe_err_lev); \
}} while(0);


#define netconn_new(t)                  netconn_new_with_proto_and_callback(t, 0, NULL)
#define netconn_new_with_callback(t, c) netconn_new_with_proto_and_callback(t, 0, c)
struct netconn *netconn_new_with_proto_and_callback(enum netconn_type t, u8_t proto,
                                             netconn_callback callback);
err_t   netconn_delete(struct netconn *conn);
/** Get the type of a netconn (as enum netconn_type). */
#define netconn_type(conn) (conn->type)

err_t   netconn_getaddr(struct netconn *conn, ip_addr_t *addr,
                        u16_t *port, u8_t local);
/** @ingroup netconn_common */
#define netconn_peer(c,i,p) netconn_getaddr(c,i,p,0)
/** @ingroup netconn_common */
#define netconn_addr(c,i,p) netconn_getaddr(c,i,p,1)

err_t   netconn_bind(struct netconn *conn, const ip_addr_t *addr, u16_t port);
err_t   netconn_connect(struct netconn *conn, const ip_addr_t *addr, u16_t port);
err_t   netconn_disconnect (struct netconn *conn);
err_t   netconn_listen_with_backlog(struct netconn *conn, u8_t backlog);
/** @ingroup netconn_tcp */
#define netconn_listen(conn) netconn_listen_with_backlog(conn, TCP_DEFAULT_LISTEN_BACKLOG)
err_t   netconn_accept(struct netconn *conn, struct netconn **new_conn);
err_t   netconn_recv(struct netconn *conn, struct netbuf **new_buf);
err_t   netconn_recv_tcp_pbuf(struct netconn *conn, struct pbuf **new_buf);
err_t   netconn_sendto(struct netconn *conn, struct netbuf *buf,
                             const ip_addr_t *addr, u16_t port);
err_t   netconn_send(struct netconn *conn, struct netbuf *buf);
err_t   netconn_write_partly(struct netconn *conn, const void *dataptr, size_t size,
                             u8_t apiflags, size_t *bytes_written);
/** @ingroup netconn_tcp */
#define netconn_write(conn, dataptr, size, apiflags) \
          netconn_write_partly(conn, dataptr, size, apiflags, NULL)
err_t   netconn_close(struct netconn *conn);
err_t   netconn_shutdown(struct netconn *conn, u8_t shut_rx, u8_t shut_tx);

#if LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD)
err_t   netconn_join_leave_group(struct netconn *conn, const ip_addr_t *multiaddr,
                             const ip_addr_t *netif_addr, enum netconn_igmp join_or_leave);
#endif /* LWIP_IGMP || (LWIP_IPV6 && LWIP_IPV6_MLD) */
#if LWIP_DNS
#if LWIP_IPV4 && LWIP_IPV6
err_t   netconn_gethostbyname_addrtype(const char *name, ip_addr_t *addr, u8_t dns_addrtype);
#define netconn_gethostbyname(name, addr) netconn_gethostbyname_addrtype(name, addr, NETCONN_DNS_DEFAULT)
#else /* LWIP_IPV4 && LWIP_IPV6 */
err_t   netconn_gethostbyname(const char *name, ip_addr_t *addr);
#define netconn_gethostbyname_addrtype(name, addr, dns_addrtype) netconn_gethostbyname(name, addr)
#endif /* LWIP_IPV4 && LWIP_IPV6 */
#endif /* LWIP_DNS */

#define netconn_err(conn)               ((conn)->last_err)
#define netconn_recv_bufsize(conn)      ((conn)->recv_bufsize)

/** Set the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_set_nonblocking(conn, val)  do { if(val) { \
  (conn)->flags |= NETCONN_FLAG_NON_BLOCKING; \
} else { \
  (conn)->flags &= ~ NETCONN_FLAG_NON_BLOCKING; }} while(0)
/** Get the blocking status of netconn calls (@todo: write/send is missing) */
#define netconn_is_nonblocking(conn)        (((conn)->flags & NETCONN_FLAG_NON_BLOCKING) != 0)

#if LWIP_IPV6
/** @ingroup netconn_common
 * TCP: Set the IPv6 ONLY status of netconn calls (see NETCONN_FLAG_IPV6_V6ONLY)
 */
#define netconn_set_ipv6only(conn, val)  do { if(val) { \
  (conn)->flags |= NETCONN_FLAG_IPV6_V6ONLY; \
} else { \
  (conn)->flags &= ~ NETCONN_FLAG_IPV6_V6ONLY; }} while(0)
/** @ingroup netconn_common
 * TCP: Get the IPv6 ONLY status of netconn calls (see NETCONN_FLAG_IPV6_V6ONLY)
 */
#define netconn_get_ipv6only(conn)        (((conn)->flags & NETCONN_FLAG_IPV6_V6ONLY) != 0)
#endif /* LWIP_IPV6 */

#if LWIP_SO_SNDTIMEO
/** Set the send timeout in milliseconds */
#define netconn_set_sendtimeout(conn, timeout)      ((conn)->send_timeout = (timeout))
/** Get the send timeout in milliseconds */
#define netconn_get_sendtimeout(conn)               ((conn)->send_timeout)
#endif /* LWIP_SO_SNDTIMEO */
#if LWIP_SO_RCVTIMEO
/** Set the receive timeout in milliseconds */
#define netconn_set_recvtimeout(conn, timeout)      ((conn)->recv_timeout = (timeout))
/** Get the receive timeout in milliseconds */
#define netconn_get_recvtimeout(conn)               ((conn)->recv_timeout)
#endif /* LWIP_SO_RCVTIMEO */
#if LWIP_SO_RCVBUF
/** Set the receive buffer in bytes */
#define netconn_set_recvbufsize(conn, recvbufsize)  ((conn)->recv_bufsize = (recvbufsize))
/** Get the receive buffer in bytes */
#define netconn_get_recvbufsize(conn)               ((conn)->recv_bufsize)
#endif /* LWIP_SO_RCVBUF*/

#if LWIP_NETCONN_SEM_PER_THREAD
void netconn_thread_init(void);
void netconn_thread_cleanup(void);
#else /* LWIP_NETCONN_SEM_PER_THREAD */
#define netconn_thread_init()
#define netconn_thread_cleanup()
#endif /* LWIP_NETCONN_SEM_PER_THREAD */

#ifdef __cplusplus
}
#endif

#endif /* LWIP_NETCONN || LWIP_SOCKET */

#endif /* LWIP_HDR_API_H */
