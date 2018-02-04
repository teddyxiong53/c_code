

#ifndef LWIP_HDR_ERR_H
#define LWIP_HDR_ERR_H

#include "lwip/opt.h"
#include "lwip/arch.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef s8_t err_t;


/** Definitions for error constants. */
typedef enum {
/** No error, everything OK. */
  ERR_OK         = 0,
/** Out of memory error.     */
  ERR_MEM        = -1,
/** Buffer error.            */
  ERR_BUF        = -2,
/** Timeout.                 */
  ERR_TIMEOUT    = -3,
/** Routing problem.         */
  ERR_RTE        = -4,
/** Operation in progress    */
  ERR_INPROGRESS = -5,
/** Illegal value.           */
  ERR_VAL        = -6,
/** Operation would block.   */
  ERR_WOULDBLOCK = -7,
/** Address in use.          */
  ERR_USE        = -8,
/** Already connecting.      */
  ERR_ALREADY    = -9,
/** Conn already established.*/
  ERR_ISCONN     = -10,
/** Not connected.           */
  ERR_CONN       = -11,
/** Low-level netif error    */
  ERR_IF         = -12,

/** Connection aborted.      */
  ERR_ABRT       = -13,
/** Connection reset.        */
  ERR_RST        = -14,
/** Connection closed.       */
  ERR_CLSD       = -15,
/** Illegal argument.        */
  ERR_ARG        = -16
} err_enum_t;

#define ERR_IS_FATAL(e) ((e) <= ERR_ABRT)


#ifdef LWIP_DEBUG
extern const char *lwip_strerr(err_t err);
#else
#define lwip_strerr(x) ""
#endif /* LWIP_DEBUG */


int err_to_errno(err_t err);


#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_ERR_H */
