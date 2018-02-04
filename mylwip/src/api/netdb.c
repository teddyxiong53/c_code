#include "lwip/netdb.h"

#include "lwip/err.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/api.h"
#include "lwip/dns.h"

#include <string.h> /* memset */
#include <stdlib.h> /* atoi */

struct hostent*
lwip_gethostbyname(const char *name)
{
    return NULL;
}

int
lwip_gethostbyname_r(const char *name, struct hostent *ret, char *buf,
                size_t buflen, struct hostent **result, int *h_errnop)
{
    return 0;
}


void
lwip_freeaddrinfo(struct addrinfo *ai)
{
    
}

int
lwip_getaddrinfo(const char *nodename, const char *servname,
       const struct addrinfo *hints, struct addrinfo **res)
{
    
}


