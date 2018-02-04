#ifndef __NETIF_ETHERNETIF_H__
#define __NETIF_ETHERNETIF_H__

#include "lwip/netif.h"
#include <rtthread.h>

#define NIOCTL_GADDR		0x01

#define ETHERNET_MTU		1500


#define ETHIF_LINK_AUTOUP	0x0000
#define ETHIF_LINK_PHYUP	0x0100

struct eth_device
{
	struct rt_device parent;

	struct netif *netif;
	struct rt_semaphore tx_ack;

	rt_uint16_t flags;
	rt_uint8_t  link_changed;
	rt_uint8_t  link_status;


	struct pbuf* (*eth_rx)(rt_device_t dev);
	rt_err_t (*eth_tx)(rt_device_t dev, struct pbuf* p);
};

rt_err_t eth_device_ready(struct eth_device* dev);
rt_err_t eth_device_init(struct eth_device * dev, char *name);
rt_err_t eth_device_init_with_flag(struct eth_device *dev, char *name, rt_uint16_t flag);
rt_err_t eth_device_linkchange(struct eth_device* dev, rt_bool_t up);

int eth_system_device_init(void);

#endif /* __NETIF_ETHERNETIF_H__ */
