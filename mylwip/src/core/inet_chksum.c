#include "lwip/opt.h"

#include "lwip/inet_chksum.h"
#include "lwip/def.h"
#include "lwip/ip_addr.h"

#include <string.h>


u16_t
inet_chksum(const void *dataptr, u16_t len)
{
	return (u16_t)~(unsigned int)LWIP_CHKSUM(dataptr, len);
}

u16_t
inet_chksum_pbuf(struct pbuf *p)
{
	u32_t acc;
	struct pbuf *q;
	u8_t swapped;

	acc = 0;
	swapped = 0;
	for (q = p; q != NULL; q = q->next)
	{
		acc += LWIP_CHKSUM(q->payload, q->len);
		acc = FOLD_U32T(acc);
		if (q->len % 2 != 0)
		{
			swapped = 1 - swapped;
			acc = SWAP_BYTES_IN_WORD(acc);
		}
	}

	if (swapped)
	{
		acc = SWAP_BYTES_IN_WORD(acc);
	}
	return (u16_t)~(acc & 0xffffUL);
}


u16_t
inet_chksum_pseudo(struct pbuf *p, u8_t proto, u16_t proto_len,
                   const ip4_addr_t *src, const ip4_addr_t *dest)
{
	u32_t acc;
	u32_t addr;

	addr = ip4_addr_get_u32(src);
	acc = (addr & 0xffffUL);
	acc += ((addr >> 16) & 0xffffUL);
	addr = ip4_addr_get_u32(dest);
	acc += (addr & 0xffffUL);
	acc += ((addr >> 16) & 0xffffUL);
	/* fold down to 16 bits */
	acc = FOLD_U32T(acc);
	acc = FOLD_U32T(acc);

	return inet_cksum_pseudo_base(p, proto, proto_len, acc);
}


static u16_t
inet_cksum_pseudo_partial_base(struct pbuf *p, u8_t proto, u16_t proto_len,
       u16_t chksum_len, u32_t acc)
{
  struct pbuf *q;
  u8_t swapped = 0;
  u16_t chklen;

  /* iterate through all pbuf in chain */
  for (q = p; (q != NULL) && (chksum_len > 0); q = q->next) {
    LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): checksumming pbuf %p (has next %p) \n",
      (void *)q, (void *)q->next));
    chklen = q->len;
    if (chklen > chksum_len) {
      chklen = chksum_len;
    }
    acc += LWIP_CHKSUM(q->payload, chklen);
    chksum_len -= chklen;
    LWIP_ASSERT("delete me", chksum_len < 0x7fff);
    /*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): unwrapped lwip_chksum()=%"X32_F" \n", acc));*/
    /* fold the upper bit down */
    acc = FOLD_U32T(acc);
    if (q->len % 2 != 0) {
      swapped = 1 - swapped;
      acc = SWAP_BYTES_IN_WORD(acc);
    }
    /*LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): wrapped lwip_chksum()=%"X32_F" \n", acc));*/
  }

  if (swapped) {
    acc = SWAP_BYTES_IN_WORD(acc);
  }

  acc += (u32_t)lwip_htons((u16_t)proto);
  acc += (u32_t)lwip_htons(proto_len);

  /* Fold 32-bit sum to 16 bits
     calling this twice is probably faster than if statements... */
  acc = FOLD_U32T(acc);
  acc = FOLD_U32T(acc);
  LWIP_DEBUGF(INET_DEBUG, ("inet_chksum_pseudo(): pbuf chain lwip_chksum()=%"X32_F"\n", acc));
  return (u16_t)~(acc & 0xffffUL);
}

u16_t
inet_chksum_pseudo_partial(struct pbuf *p, u8_t proto, u16_t proto_len,
                           u16_t chksum_len, const ip4_addr_t *src, const ip4_addr_t *dest)
{
	u32_t acc;
	u32_t addr;

	addr = ip4_addr_get_u32(src);
	acc = (addr & 0xffffUL);
	acc += ((addr >> 16) & 0xffffUL);
	addr = ip4_addr_get_u32(dest);
	acc += (addr & 0xffffUL);
	acc += ((addr >> 16) & 0xffffUL);
	/* fold down to 16 bits */
	acc = FOLD_U32T(acc);
	acc = FOLD_U32T(acc);

	return inet_cksum_pseudo_partial_base(p, proto, proto_len, chksum_len, acc);
}


u16_t
ip_chksum_pseudo(struct pbuf *p, u8_t proto, u16_t proto_len,
                 const ip_addr_t *src, const ip_addr_t *dest)
{

	return inet_chksum_pseudo(p, proto, proto_len, ip_2_ip4(src), ip_2_ip4(dest));

}

u16_t
ip_chksum_pseudo_partial(struct pbuf *p, u8_t proto, u16_t proto_len,
                         u16_t chksum_len, const ip_addr_t *src, const ip_addr_t *dest)
{

	return inet_chksum_pseudo_partial(p, proto, proto_len, chksum_len, ip_2_ip4(src), ip_2_ip4(dest));

}

