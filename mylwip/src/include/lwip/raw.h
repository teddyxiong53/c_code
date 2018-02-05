
#include "lwip/opt.h"


#include "lwip/pbuf.h"
#include "lwip/def.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"

typedef u8_t (*raw_recv_fn)(void *arg, struct raw_pcb *pcb, struct pbuf *p, const ip_addr_t *addr);
struct raw_pcb 
{
    ip_addr_t local_ip;
    ip_addr_t remote_ip;
    u8_t so_options; 
    u8_t tos;
    u8_t ttl;

    struct raw_pcb *next;
    u8_t protocol;
    raw_recv_fn recv;
    void *recv_arg;
};
