#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include <rtconfig.h>

#define ERRNO                       1

#define LWIP_IPV4                   1


#define LWIP_IPV6                   0


#define NO_SYS                      0
#define LWIP_SOCKET                 1
#define LWIP_NETCONN                1


#define LWIP_IGMP                   0



#define LWIP_ICMP                   1



#define LWIP_SNMP                   0



#define LWIP_DNS                    1


#define LWIP_HAVE_LOOPIF            1

#define LWIP_PLATFORM_BYTESWAP      0

#ifndef BYTE_ORDER
#define BYTE_ORDER                  LITTLE_ENDIAN
#endif

#define RT_LWIP_DEBUG 

#ifdef RT_LWIP_DEBUG
#define LWIP_DEBUG
#endif

/* ---------- Debug options ---------- */
#ifdef LWIP_DEBUG
#define SYS_DEBUG                   LWIP_DBG_ON
#define ETHARP_DEBUG                LWIP_DBG_OFF
#define PPP_DEBUG                   LWIP_DBG_OFF
#define MEM_DEBUG                   LWIP_DBG_OFF
#define MEMP_DEBUG                  LWIP_DBG_ON
#define PBUF_DEBUG                  LWIP_DBG_ON
#define API_LIB_DEBUG               LWIP_DBG_ON
#define API_MSG_DEBUG               LWIP_DBG_ON
#define TCPIP_DEBUG                 LWIP_DBG_ON
#define NETIF_DEBUG                 LWIP_DBG_ON
#define SOCKETS_DEBUG               LWIP_DBG_ON
#define DNS_DEBUG                   LWIP_DBG_OFF
#define AUTOIP_DEBUG                LWIP_DBG_OFF
#define DHCP_DEBUG                  LWIP_DBG_OFF
#define IP_DEBUG                    LWIP_DBG_OFF
#define IP_REASS_DEBUG              LWIP_DBG_OFF
#define ICMP_DEBUG                  LWIP_DBG_ON
#define IGMP_DEBUG                  LWIP_DBG_OFF
#define UDP_DEBUG                   LWIP_DBG_OFF
#define TCP_DEBUG                   LWIP_DBG_ON
#define TCP_INPUT_DEBUG             LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG            LWIP_DBG_ON
#define TCP_RTO_DEBUG               LWIP_DBG_ON
#define TCP_CWND_DEBUG              LWIP_DBG_ON
#define TCP_WND_DEBUG               LWIP_DBG_OFF
#define TCP_FR_DEBUG                LWIP_DBG_OFF
#define TCP_QLEN_DEBUG              LWIP_DBG_OFF
#define TCP_RST_DEBUG               LWIP_DBG_OFF
#endif

#define LWIP_DBG_TYPES_ON           (LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)

/* ---------- Memory options ---------- */
#define MEM_ALIGNMENT               4
#define MEMP_OVERFLOW_CHECK         1 ////
#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 1 ////
//#define MEM_LIBC_MALLOC             1
//#define MEM_USE_POOLS               1
//#define MEMP_USE_CUSTOM_POOLS       1
//#define MEM_SIZE                    (1024*64)

#define MEMP_MEM_MALLOC             0


#define MEMP_NUM_PBUF               32 //16

/* the number of struct netconns */
#ifdef RT_MEMP_NUM_NETCONN
#define MEMP_NUM_NETCONN            RT_MEMP_NUM_NETCONN
#endif

/* the number of UDP protocol control blocks. One per active RAW "connection". */
#ifdef RT_LWIP_RAW_PCB_NUM
#define MEMP_NUM_RAW_PCB            RT_LWIP_RAW_PCB_NUM
#endif

/* the number of UDP protocol control blocks. One per active UDP "connection". */
#ifdef RT_LWIP_UDP_PCB_NUM
#define MEMP_NUM_UDP_PCB            RT_LWIP_UDP_PCB_NUM
#endif

/* the number of simulatenously active TCP connections. */
#ifdef RT_LWIP_TCP_PCB_NUM
#define MEMP_NUM_TCP_PCB            RT_LWIP_TCP_PCB_NUM
#endif

/* the number of simultaneously queued TCP */
#ifdef RT_LWIP_TCP_SEG_NUM
#define MEMP_NUM_TCP_SEG            RT_LWIP_TCP_SEG_NUM
#else
#define MEMP_NUM_TCP_SEG            TCP_SND_QUEUELEN
#endif



/* ---------- Pbuf options ---------- */
/* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
#ifdef RT_LWIP_PBUF_NUM
#define PBUF_POOL_SIZE               RT_LWIP_PBUF_NUM
#endif

/* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
#ifdef RT_LWIP_PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE            RT_LWIP_PBUF_POOL_BUFSIZE
#endif


#define PBUF_LINK_HLEN              16

#ifdef RT_LWIP_ETH_PAD_SIZE
#define ETH_PAD_SIZE                RT_LWIP_ETH_PAD_SIZE
#endif


#define SYS_LIGHTWEIGHT_PROT        (NO_SYS==0)


#define IP_NAT                      0



#define LWIP_TCP                    0


#define TCP_TTL                     255


#define TCP_QUEUE_OOSEQ             1


#define TCP_MSS                     1460


#ifdef RT_LWIP_TCP_SND_BUF
#define TCP_SND_BUF                 RT_LWIP_TCP_SND_BUF
#else
#define TCP_SND_BUF                 (TCP_MSS * 2)
#endif


#define TCP_SND_QUEUELEN            (4 * TCP_SND_BUF/TCP_MSS)

#define TCP_SNDLOWAT                (TCP_SND_BUF/2)
#define TCP_SNDQUEUELOWAT           TCP_SND_QUEUELEN/2

/* TCP receive window. */
#ifdef RT_LWIP_TCP_WND
#define TCP_WND                     RT_LWIP_TCP_WND
#else
#define TCP_WND                     (TCP_MSS * 2)
#endif

/* Maximum number of retransmissions of data segments. */
#define TCP_MAXRTX                  12

/* Maximum number of retransmissions of SYN segments. */
#define TCP_SYNMAXRTX               4

/* tcpip thread options */
#ifdef RT_LWIP_TCPTHREAD_PRIORITY
#define TCPIP_MBOX_SIZE             RT_LWIP_TCPTHREAD_MBOX_SIZE
#define TCPIP_THREAD_PRIO           RT_LWIP_TCPTHREAD_PRIORITY
#define TCPIP_THREAD_STACKSIZE      RT_LWIP_TCPTHREAD_STACKSIZE
#else
#define TCPIP_MBOX_SIZE             8
#define TCPIP_THREAD_PRIO           128
#define TCPIP_THREAD_STACKSIZE      4096
#endif
#define TCPIP_THREAD_NAME           "tcpip"
#define DEFAULT_TCP_RECVMBOX_SIZE   10


#define LWIP_ARP                    0// 1
#define ARP_TABLE_SIZE              10
#define ARP_QUEUEING                1


#define IP_FORWARD                  0

/* IP reassembly and segmentation.These are orthogonal even
 * if they both deal with IP fragments */
#ifdef RT_LWIP_REASSEMBLY_FRAG
#define IP_REASSEMBLY               1
#define IP_FRAG                     1
#define IP_REASS_MAX_PBUFS          10
#define MEMP_NUM_REASSDATA          10
#else
#define IP_REASSEMBLY               0
#define IP_FRAG                     0
#endif


#define ICMP_TTL                    255



#define LWIP_DHCP                   0


/* 1 if you want to do an ARP check on the offered address
   (recommended). */
#define DHCP_DOES_ARP_CHECK         (LWIP_DHCP)


#define LWIP_AUTOIP                 0
#define LWIP_DHCP_AUTOIP_COOP       (LWIP_DHCP && LWIP_AUTOIP)


#ifdef RT_LWIP_UDP
#define LWIP_UDP                    1
#else
#define LWIP_UDP                    0
#endif

#define LWIP_UDPLITE                0
#define UDP_TTL                     255
#define DEFAULT_UDP_RECVMBOX_SIZE   1

/* ---------- RAW options ---------- */
#ifdef RT_LWIP_RAW
#define LWIP_RAW                    1
#else
#define LWIP_RAW                    0
#endif

#define DEFAULT_RAW_RECVMBOX_SIZE   1
#define DEFAULT_ACCEPTMBOX_SIZE     10

/* ---------- Statistics options ---------- */
#ifdef RT_LWIP_STATS
#define LWIP_STATS                  1
#define LWIP_STATS_DISPLAY          1
#else
#define LWIP_STATS                  0
#endif

#if LWIP_STATS
#define LINK_STATS                  1
#define IP_STATS                    1
#define ICMP_STATS                  1
#define IGMP_STATS                  1
#define IPFRAG_STATS                1
#define UDP_STATS                   1
#define TCP_STATS                   1
#define MEM_STATS                   1
#define MEMP_STATS                  1
#define PBUF_STATS                  1
#define SYS_STATS                   1
#endif /* LWIP_STATS */

/* ---------- PPP options ---------- */
#ifdef RT_LWIP_PPP
#define PPP_SUPPORT                 1      /* Set > 0 for PPP */
#else
#define PPP_SUPPORT                 0      /* Set > 0 for PPP */
#endif



#ifndef LWIP_POSIX_SOCKETS_IO_NAMES
#define LWIP_POSIX_SOCKETS_IO_NAMES     0
#endif


#ifndef LWIP_TCP_KEEPALIVE
#define LWIP_TCP_KEEPALIVE              1
#endif

#ifndef LWIP_NETIF_API
#define LWIP_NETIF_API                  1
#endif

#ifdef LWIP_IGMP
#include <stdlib.h>
#define LWIP_RAND                  rand
#endif

#ifndef LWIP_SOCKET
#define LWIP_SOCKET                     1
#endif


#ifdef RT_USING_DFS_NET
#define LWIP_COMPAT_SOCKETS             0
#else
#ifndef LWIP_COMPAT_SOCKETS
#define LWIP_COMPAT_SOCKETS             1
#endif
#endif


#ifndef LWIP_SO_SNDTIMEO
#define LWIP_SO_SNDTIMEO                1
#endif


#ifndef LWIP_SO_RCVTIMEO
#define LWIP_SO_RCVTIMEO                1
#endif


#ifndef LWIP_SO_RCVBUF
#define LWIP_SO_RCVBUF                  1
#endif


#ifndef RECV_BUFSIZE_DEFAULT
#define RECV_BUFSIZE_DEFAULT            8192
#endif


#ifndef SO_REUSE
#define SO_REUSE                        0
#endif


#ifdef RT_LWIP_TFTP_MAX_FILENAME_LEN
#define TFTP_MAX_FILENAME_LEN           RT_LWIP_TFTP_MAX_FILENAME_LEN
#elif defined(RT_DFS_ELM_MAX_LFN)
#define TFTP_MAX_FILENAME_LEN           RT_DFS_ELM_MAX_LFN
#else
#define TFTP_MAX_FILENAME_LEN           64
#endif


#endif /* __LWIPOPTS_H__ */
