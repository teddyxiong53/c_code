
#ifndef LWIP_HDR_INIT_H
#define LWIP_HDR_INIT_H

#include "lwip/opt.h"

#ifdef __cplusplus
extern "C" {
#endif




#define LWIP_VERSION_MAJOR      2U
#define LWIP_VERSION_MINOR      0U
#define LWIP_VERSION_REVISION   2U
#define LWIP_VERSION_RC         LWIP_RC_RELEASE


#define LWIP_RC_RELEASE         255U

#define LWIP_RC_DEVELOPMENT     0U

#define LWIP_VERSION_IS_RELEASE     (LWIP_VERSION_RC == LWIP_RC_RELEASE)
#define LWIP_VERSION_IS_DEVELOPMENT (LWIP_VERSION_RC == LWIP_RC_DEVELOPMENT)
#define LWIP_VERSION_IS_RC          ((LWIP_VERSION_RC != LWIP_RC_RELEASE) && (LWIP_VERSION_RC != LWIP_RC_DEVELOPMENT))

/* Some helper defines to get a version string */
#define LWIP_VERSTR2(x) #x
#define LWIP_VERSTR(x) LWIP_VERSTR2(x)
#if LWIP_VERSION_IS_RELEASE
#define LWIP_VERSION_STRING_SUFFIX ""
#elif LWIP_VERSION_IS_DEVELOPMENT
#define LWIP_VERSION_STRING_SUFFIX "d"
#else
#define LWIP_VERSION_STRING_SUFFIX "rc" LWIP_VERSTR(LWIP_VERSION_RC)
#endif


#define LWIP_VERSION   (LWIP_VERSION_MAJOR << 24   | LWIP_VERSION_MINOR << 16 | \
                        LWIP_VERSION_REVISION << 8 | LWIP_VERSION_RC)

#define LWIP_VERSION_STRING     LWIP_VERSTR(LWIP_VERSION_MAJOR) "." LWIP_VERSTR(LWIP_VERSION_MINOR) "." LWIP_VERSTR(LWIP_VERSION_REVISION) LWIP_VERSION_STRING_SUFFIX

/**
 * @}
 */

/* Modules initialization */
void lwip_init(void);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_INIT_H */
