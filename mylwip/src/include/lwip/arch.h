
#ifndef LWIP_HDR_ARCH_H
#define LWIP_HDR_ARCH_H

#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#endif

#ifndef BIG_ENDIAN
#define BIG_ENDIAN 4321
#endif

#include "arch/cc.h"


#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#ifdef __DOXYGEN__
#define LWIP_RAND() ((u32_t)rand())
#endif


#ifndef LWIP_PLATFORM_DIAG
#define LWIP_PLATFORM_DIAG(x) do {printf x;} while(0)
#include <stdio.h>
#include <stdlib.h>
#endif


#ifndef LWIP_PLATFORM_ASSERT
#define LWIP_PLATFORM_ASSERT(x) do {printf("Assertion \"%s\" failed at line %d in %s\n", \
                                     x, __LINE__, __FILE__); fflush(NULL); abort();} while(0)
#include <stdio.h>
#include <stdlib.h>
#endif


#ifndef LWIP_NO_STDDEF_H
#define LWIP_NO_STDDEF_H 0
#endif

#if !LWIP_NO_STDDEF_H
#include <stddef.h> /* for size_t */
#endif


#ifndef LWIP_NO_STDINT_H
#define LWIP_NO_STDINT_H 0
#endif

/* Define generic types used in lwIP */
#if !LWIP_NO_STDINT_H
#include <stdint.h>
typedef uint8_t   u8_t;
typedef int8_t    s8_t;
typedef uint16_t  u16_t;
typedef int16_t   s16_t;
typedef uint32_t  u32_t;
typedef int32_t   s32_t;
typedef uintptr_t mem_ptr_t;
#endif

/** Define this to 1 in arch/cc.h of your port if your compiler does not provide
 * the inttypes.h header. You need to define the format strings listed in
 * lwip/arch.h yourself in this case (X8_F, U16_F...).
 */
#ifndef LWIP_NO_INTTYPES_H
#define LWIP_NO_INTTYPES_H 0
#endif

/* Define (sn)printf formatters for these lwIP types */
#if !LWIP_NO_INTTYPES_H
#include <inttypes.h>
#ifndef X8_F
#define X8_F  "02" PRIx8
#endif
#ifndef U16_F
#define U16_F PRIu16
#endif
#ifndef S16_F
#define S16_F PRId16
#endif
#ifndef X16_F
#define X16_F PRIx16
#endif
#ifndef U32_F
#define U32_F PRIu32
#endif
#ifndef S32_F
#define S32_F PRId32
#endif
#ifndef X32_F
#define X32_F PRIx32
#endif
#ifndef SZT_F
#define SZT_F PRIuPTR
#endif
#endif


#ifndef LWIP_NO_LIMITS_H
#define LWIP_NO_LIMITS_H 0
#endif

/* Include limits.h? */
#if !LWIP_NO_LIMITS_H
#include <limits.h>
#endif

#ifndef LWIP_CONST_CAST
#define LWIP_CONST_CAST(target_type, val) ((target_type)((ptrdiff_t)val))
#endif


#ifndef LWIP_ALIGNMENT_CAST
#define LWIP_ALIGNMENT_CAST(target_type, val) LWIP_CONST_CAST(target_type, val)
#endif


#ifndef LWIP_PTR_NUMERIC_CAST
#define LWIP_PTR_NUMERIC_CAST(target_type, val) LWIP_CONST_CAST(target_type, val)
#endif


#ifndef LWIP_DECLARE_MEMORY_ALIGNED
#define LWIP_DECLARE_MEMORY_ALIGNED(variable_name, size) u8_t variable_name[LWIP_MEM_ALIGN_BUFFER(size)]

#endif

#ifndef LWIP_MEM_ALIGN_SIZE
#define LWIP_MEM_ALIGN_SIZE(size) (((size) + MEM_ALIGNMENT - 1U) & ~(MEM_ALIGNMENT-1U))
#endif


#ifndef LWIP_MEM_ALIGN_BUFFER
#define LWIP_MEM_ALIGN_BUFFER(size) (((size) + MEM_ALIGNMENT - 1U))
#endif


#ifndef LWIP_MEM_ALIGN
#define LWIP_MEM_ALIGN(addr) ((void *)(((mem_ptr_t)(addr) + MEM_ALIGNMENT - 1) & ~(mem_ptr_t)(MEM_ALIGNMENT-1)))
#endif

#ifdef __cplusplus
extern "C" {
#endif







#ifndef LWIP_UNUSED_ARG
#define LWIP_UNUSED_ARG(x) (void)x
#endif /* LWIP_UNUSED_ARG */



#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_ARCH_H */
