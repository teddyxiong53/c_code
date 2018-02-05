#include "lwip/opt.h"

#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/stats.h"

#include <string.h>

/* Make sure we include everything we need for size calculation required by memp_std.h */
#include "lwip/pbuf.h"
//#include "lwip/raw.h"
//#include "lwip/udp.h"
//#include "lwip/tcp.h"
//#include "lwip/priv/tcp_priv.h"
//#include "lwip/ip4_frag.h"
#include "lwip/netbuf.h"
#include "lwip/api.h"
#include "lwip/priv/tcpip_priv.h"
//#include "lwip/priv/api_msg.h"
#include "lwip/sockets.h"
#include "lwip/netifapi.h"
//#include "lwip/etharp.h"
//#include "lwip/igmp.h"
#include "lwip/timeouts.h"
/* needed by default MEMP_NUM_SYS_TIMEOUT */
//#include "netif/ppp/ppp_opts.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
//#include "lwip/priv/nd6_priv.h"
//#include "lwip/ip6_frag.h"
//#include "lwip/mld6.h"


#define LWIP_MEMPOOL(name,num,size,desc) LWIP_MEMPOOL_DECLARE(name,num,size,desc)
#include "lwip/priv/memp_std.h"

const struct memp_desc* const memp_pools[MEMP_MAX] = {
#define LWIP_MEMPOOL(name,num,size,desc) &memp_ ## name,
#include "lwip/priv/memp_std.h"
};



static void
memp_overflow_check_element_overflow(struct memp *p, const struct memp_desc *desc)
{
  u16_t k;
  u8_t *m;
  m = (u8_t*)p + MEMP_SIZE + desc->size;
  for (k = 0; k < MEMP_SANITY_REGION_AFTER_ALIGNED; k++) {
    if (m[k] != 0xcd) {
      char errstr[128] = "detected memp overflow in pool ";
      strcat(errstr, desc->desc);
      LWIP_ASSERT(errstr, 0);
    }
  }
}


static void
memp_overflow_check_element_underflow(struct memp *p, const struct memp_desc *desc)
{
  u16_t k;
  u8_t *m;
  m = (u8_t*)p + MEMP_SIZE - MEMP_SANITY_REGION_BEFORE_ALIGNED;
  for (k = 0; k < MEMP_SANITY_REGION_BEFORE_ALIGNED; k++) {
    if (m[k] != 0xcd) {
      char errstr[128] = "detected memp underflow in pool ";
      strcat(errstr, desc->desc);
      LWIP_ASSERT(errstr, 0);
    }
  }
}


static void
memp_overflow_init_element(struct memp *p, const struct memp_desc *desc)
{
  u8_t *m;
  m = (u8_t*)p + MEMP_SIZE - MEMP_SANITY_REGION_BEFORE_ALIGNED;
  memset(m, 0xcd, MEMP_SANITY_REGION_BEFORE_ALIGNED);

  m = (u8_t*)p + MEMP_SIZE + desc->size;
  memset(m, 0xcd, MEMP_SANITY_REGION_AFTER_ALIGNED);
}

void memp_init_pool(const struct memp_desc *desc)
{

  int i;
  struct memp *memp;

  *desc->tab = NULL;
  memp = (struct memp*)LWIP_MEM_ALIGN(desc->base);
  /* create a linked list of memp elements */
  for (i = 0; i < desc->num; ++i) {
    memp->next = *desc->tab;
    *desc->tab = memp;
    memp_overflow_init_element(memp, desc);
   memp = (struct memp *)(void *)((u8_t *)memp + MEMP_SIZE + desc->size

      + MEMP_SANITY_REGION_AFTER_ALIGNED

    );
  }
}



void memp_init(void)
{
    u16_t i;
    for(i=0; i<LWIP_ARRAYSIZE(memp_pools); i++)
    {
        
    }
}




static void* do_memp_malloc_pool_fn(const struct memp_desc *desc, const char* file, const int line)

{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  SYS_ARCH_PROTECT(old_level);

  memp = *desc->tab;

  if (memp != NULL) {


    memp_overflow_check_element_overflow(memp, desc);
    memp_overflow_check_element_underflow(memp, desc);

    *desc->tab = memp->next;
    memp->next = NULL;


    memp->file = file;
    memp->line = line;


    LWIP_ASSERT("memp_malloc: memp properly aligned",
                ((mem_ptr_t)memp % MEM_ALIGNMENT) == 0);
    SYS_ARCH_UNPROTECT(old_level);
    /* cast through u8_t* to get rid of alignment warnings */
    return ((u8_t*)memp + MEMP_SIZE);
  } else {

  }

  SYS_ARCH_UNPROTECT(old_level);
  return NULL;
}






void *memp_malloc_pool_fn(const struct memp_desc *desc, const char* file, const int line)

{
  LWIP_ASSERT("invalid pool desc", desc != NULL);
  if (desc == NULL) {
    return NULL;
  }
  return do_memp_malloc_pool_fn(desc, file, line);
}





void *memp_malloc_fn(memp_t type, const char* file, const int line)

{
  void *memp;


  memp = do_memp_malloc_pool_fn(memp_pools[type], file, line);


  return memp;
}




static void do_memp_free_pool(const struct memp_desc* desc, void *mem)
{
  struct memp *memp;
  SYS_ARCH_DECL_PROTECT(old_level);

  memp = (struct memp *)(void *)((u8_t*)mem - MEMP_SIZE);

  SYS_ARCH_PROTECT(old_level);

  memp_overflow_check_element_overflow(memp, desc);
  memp_overflow_check_element_underflow(memp, desc);




  memp->next = *desc->tab;
  *desc->tab = memp;


  SYS_ARCH_UNPROTECT(old_level);

}

void memp_free_pool(const struct memp_desc* desc, void *mem)
{
  if ((desc == NULL) || (mem == NULL)) {
    return;
  }

  do_memp_free_pool(desc, mem);
}





void memp_free(memp_t type, void *mem)
{


  if (mem == NULL) {
    return;
  }

  do_memp_free_pool(memp_pools[type], mem);

}

