
#ifndef LWIP_HDR_DNS_H
#define LWIP_HDR_DNS_H

#include "lwip/opt.h"

#if LWIP_DNS
#include "lwip/err.h"
#include "lwip/ip_addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/** DNS timer period */
#define DNS_TMR_INTERVAL          1000

/* DNS resolve types: */
#define LWIP_DNS_ADDRTYPE_IPV4      0
#define LWIP_DNS_ADDRTYPE_IPV6      1
#define LWIP_DNS_ADDRTYPE_IPV4_IPV6 2 /* try to resolve IPv4 first, try IPv6 if IPv4 fails only */
#define LWIP_DNS_ADDRTYPE_IPV6_IPV4 3 /* try to resolve IPv6 first, try IPv4 if IPv6 fails only */


#define LWIP_DNS_ADDRTYPE_DEFAULT   LWIP_DNS_ADDRTYPE_IPV4




extern const ip_addr_t dns_mquery_v4group;



typedef void (*dns_found_callback)(const char *name, const ip_addr_t *ipaddr, void *callback_arg);

void             dns_init(void);
void             dns_tmr(void);
void             dns_setserver(u8_t numdns, const ip_addr_t *dnsserver);
const ip_addr_t* dns_getserver(u8_t numdns);
err_t            dns_gethostbyname(const char *hostname, ip_addr_t *addr,
                                   dns_found_callback found, void *callback_arg);
err_t            dns_gethostbyname_addrtype(const char *hostname, ip_addr_t *addr,
                                   dns_found_callback found, void *callback_arg,
                                   u8_t dns_addrtype);



#ifdef __cplusplus
}
#endif

#endif /* LWIP_DNS */

#endif /* LWIP_HDR_DNS_H */
