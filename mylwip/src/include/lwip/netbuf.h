
#ifndef LWIP_HDR_NETBUF_H
#define LWIP_HDR_NETBUF_H

#include "lwip/opt.h"

#include "lwip/pbuf.h"
#include "lwip/ip_addr.h"


#ifdef __cplusplus
extern "C" {
#endif

#define NETBUF_FLAG_DESTADDR    0x01
#define NETBUF_FLAG_CHKSUM      0x02

struct netbuf {
  struct pbuf *p, *ptr;
  ip_addr_t addr;
  u16_t port;
};

/* Network buffer functions: */
struct netbuf *   netbuf_new      (void);
void              netbuf_delete   (struct netbuf *buf);
void *            netbuf_alloc    (struct netbuf *buf, u16_t size);
void              netbuf_free     (struct netbuf *buf);
err_t             netbuf_ref      (struct netbuf *buf,
                                   const void *dataptr, u16_t size);
void              netbuf_chain    (struct netbuf *head, struct netbuf *tail);

err_t             netbuf_data     (struct netbuf *buf,
                                   void **dataptr, u16_t *len);
s8_t              netbuf_next     (struct netbuf *buf);
void              netbuf_first    (struct netbuf *buf);


#define netbuf_copy_partial(buf, dataptr, len, offset) \
  pbuf_copy_partial((buf)->p, (dataptr), (len), (offset))
#define netbuf_copy(buf,dataptr,len) netbuf_copy_partial(buf, dataptr, len, 0)
#define netbuf_take(buf, dataptr, len) pbuf_take((buf)->p, dataptr, len)
#define netbuf_len(buf)              ((buf)->p->tot_len)
#define netbuf_fromaddr(buf)         (&((buf)->addr))
#define netbuf_set_fromaddr(buf, fromaddr) ip_addr_set(&((buf)->addr), fromaddr)
#define netbuf_fromport(buf)         ((buf)->port)
#if LWIP_NETBUF_RECVINFO
#define netbuf_destaddr(buf)         (&((buf)->toaddr))
#define netbuf_set_destaddr(buf, destaddr) ip_addr_set(&((buf)->toaddr), destaddr)
#if LWIP_CHECKSUM_ON_COPY
#define netbuf_destport(buf)         (((buf)->flags & NETBUF_FLAG_DESTADDR) ? (buf)->toport_chksum : 0)
#else /* LWIP_CHECKSUM_ON_COPY */
#define netbuf_destport(buf)         ((buf)->toport_chksum)
#endif /* LWIP_CHECKSUM_ON_COPY */
#endif /* LWIP_NETBUF_RECVINFO */
#if LWIP_CHECKSUM_ON_COPY
#define netbuf_set_chksum(buf, chksum) do { (buf)->flags = NETBUF_FLAG_CHKSUM; \
                                            (buf)->toport_chksum = chksum; } while(0)
#endif /* LWIP_CHECKSUM_ON_COPY */

#ifdef __cplusplus
}
#endif


#endif /* LWIP_HDR_NETBUF_H */
