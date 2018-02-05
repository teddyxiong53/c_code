#include "lwip/opt.h"


#include "lwip/netbuf.h"
#include "lwip/memp.h"

#include <string.h>

struct netbuf * nebuf_new(void)
{
    struct netbuf *buf;
    buf = (struct netbuf *)memp_malloc(MEMP_NETBUF);
    if(buf != 0)
    {
        memset(buf, 0, sizeof(*buf));
    }
    return buf;
}

void nebuf_delete(struct netbuf *buf)
{
    if(buf != NULL)
    {
        if(buf->p != NULL)
        {
            pbuf_free(buf->p);
            buf->p = buf->ptr = NULL;
        }
        memp_free(MEMP_NETBUF, buf);
    }
    
}

err_t netbuf_ref(struct netbuf *buf, const void *dataptr, u16_t size)
{
    if(buf == NULL)
    {
        myloge("buf is null");
        return ERR_ARG;
    }
    if(buf->p != NULL)
    {
        pbuf_free(buf->p);
    }
    pbuf->p = pbuf_alloc(PBUF_TRANSPORT, 0, PBUF_REF);
    if(pbuf->p == NULL)
    {
        myloge("pbuf alloc failed");
        return ERR_MEM;
    }
    buf->p->payload = dataptr;
    buf->p->len = buf->p->tot_len = size;
    buf->ptr = buf->p;
    return ERR_OK;
}

