#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/ip_addr.h"
#include "lwip/err.h"
#include "lwip/sys.h"
//#include "lwip/igmp.h"
#include "lwip/api.h"
#include "lwip/priv/tcpip_priv.h"
struct api_msg
{
    struct netconn *conn;
    err_t err;
    union {
        struct netbuf *b;
        struct {
            u8_t proto;
        }n;
        struct {
            ip_addr_t *ipaddr;
            u16_t port;
        }bc;
        
    }msg;
};

#define LWIP_API_MSG_SEM(msg) (&(msg)->conn->op_completed)
