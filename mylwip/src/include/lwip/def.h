
#ifndef LWIP_HDR_DEF_H
#define LWIP_HDR_DEF_H

#include "lwip/arch.h"
#include "lwip/opt.h"

#define PERF_START    
#define PERF_STOP(x)  


#ifdef __cplusplus
extern "C" {
#endif

#define LWIP_MAX(x , y)  (((x) > (y)) ? (x) : (y))
#define LWIP_MIN(x , y)  (((x) < (y)) ? (x) : (y))


#define LWIP_ARRAYSIZE(x) (sizeof(x)/sizeof((x)[0]))

#define LWIP_MAKEU32(a,b,c,d) (((u32_t)((a) & 0xff) << 24) | \
                               ((u32_t)((b) & 0xff) << 16) | \
                               ((u32_t)((c) & 0xff) << 8)  | \
                                (u32_t)((d) & 0xff))

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

#if BYTE_ORDER == BIG_ENDIAN
#define lwip_htons(x) (x)
#define lwip_ntohs(x) (x)
#define lwip_htonl(x) (x)
#define lwip_ntohl(x) (x)
#define PP_HTONS(x) (x)
#define PP_NTOHS(x) (x)
#define PP_HTONL(x) (x)
#define PP_NTOHL(x) (x)
#else /* BYTE_ORDER != BIG_ENDIAN */
#ifndef lwip_htons
u16_t lwip_htons(u16_t x);
#endif
#define lwip_ntohs(x) lwip_htons(x)

#ifndef lwip_htonl
u32_t lwip_htonl(u32_t x);
#endif
#define lwip_ntohl(x) lwip_htonl(x)

/* Provide usual function names as macros for users, but this can be turned off */
#ifndef LWIP_DONT_PROVIDE_BYTEORDER_FUNCTIONS
#define htons(x) lwip_htons(x)
#define ntohs(x) lwip_ntohs(x)
#define htonl(x) lwip_htonl(x)
#define ntohl(x) lwip_ntohl(x)
#endif


#define PP_HTONS(x) ((((x) & 0x00ffUL) << 8) | (((x) & 0xff00UL) >> 8))
#define PP_NTOHS(x) PP_HTONS(x)
#define PP_HTONL(x) ((((x) & 0x000000ffUL) << 24) | \
                     (((x) & 0x0000ff00UL) <<  8) | \
                     (((x) & 0x00ff0000UL) >>  8) | \
                     (((x) & 0xff000000UL) >> 24))
#define PP_NTOHL(x) PP_HTONL(x)

#endif /* BYTE_ORDER == BIG_ENDIAN */



void  lwip_itoa(char* result, size_t bufsize, int number);
int   lwip_strnicmp(const char* str1, const char* str2, size_t len);
int   lwip_stricmp(const char* str1, const char* str2);
char* lwip_strnstr(const char* buffer, const char* token, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_DEF_H */
