/**
*  @defgroup ddk_net_device
*  @ingroup DDK
*
*  定义了网络设备
*  @{
*/

#ifndef _DDK_NET_DEVICE_H_
#define _DDK_NET_DEVICE_H_
#include <stddef.h>
#include <list.h>
#include <compiler.h>

#include "features.h"
#include "define.h"

#define net_device ddk_net_device
#define header_ops ddk_net_device_header_ops

struct ddk_net_buf;
struct sk_buff;
struct ddk_net_device;

struct ddk_net_device_header_ops {
	int	(*create) (struct sk_buff *skb, struct ddk_net_device *dev,
				   unsigned short type, const void *daddr,
				   const void *saddr, unsigned int len);
	int	(*parse)(const struct sk_buff *skb, unsigned char *haddr);
	int	(*rebuild)(struct sk_buff *skb);
};

struct net_device_stats {
	unsigned long	rx_packets;
	unsigned long	tx_packets;
	unsigned long	rx_bytes;
	unsigned long	tx_bytes;
	unsigned long	rx_errors;
	unsigned long	tx_errors;
	unsigned long	rx_dropped;
	unsigned long	tx_dropped;
	unsigned long	multicast;
	unsigned long	collisions;
	unsigned long	rx_length_errors;
	unsigned long	rx_over_errors;
	unsigned long	rx_crc_errors;
	unsigned long	rx_frame_errors;
	unsigned long	rx_fifo_errors;
	unsigned long	rx_missed_errors;
	unsigned long	tx_aborted_errors;
	unsigned long	tx_carrier_errors;
	unsigned long	tx_fifo_errors;
	unsigned long	tx_heartbeat_errors;
	unsigned long	tx_window_errors;
	unsigned long	rx_compressed;
	unsigned long	tx_compressed;
};

struct ddk_net_device_ops 
{
	int							(*open)(struct ddk_net_device *dev);
	int							(*stop)(struct ddk_net_device *dev);
	netdev_tx_t					(*start_xmit) (struct ddk_net_buf *skb, struct ddk_net_device *dev);
	void						(*set_rx_mode)(struct ddk_net_device *dev);
	int							(*set_mac_address)(struct ddk_net_device *dev, void *addr);
	struct net_device_stats*	(*get_stats)(struct ddk_net_device *dev);
	int							(*change_mtu)(struct ddk_net_device *dev, int new_mtu);
	void						(*tx_timeout) (struct ddk_net_device *dev);
	int							(*set_features)(struct ddk_net_device *dev, netdev_features_t features);
};

#define MAX_ADDR_LEN	32		/* Largest hardware address length */

struct netdev_hw_addr {
	struct list_head	list;
	unsigned char		addr[MAX_ADDR_LEN];
	unsigned char		type;
#define NETDEV_HW_ADDR_T_LAN		1
#define NETDEV_HW_ADDR_T_SAN		2
#define NETDEV_HW_ADDR_T_SLAVE		3
#define NETDEV_HW_ADDR_T_UNICAST	4
#define NETDEV_HW_ADDR_T_MULTICAST	5
	bool			global_use;
	int			refcount;
	int			synced;
};

struct netdev_hw_addr_list {
	struct list_head	list;
	int			count;
};
#define netdev_hw_addr_list_count(l) ((l)->count)
#define netdev_hw_addr_list_empty(l) (netdev_hw_addr_list_count(l) == 0)
#define netdev_hw_addr_list_for_each(ha, l) \
	list_for_each_entry(ha, &(l)->list, list)
#define netdev_mc_count(dev) netdev_hw_addr_list_count(&(dev)->mc)
#define netdev_for_each_mc_addr(ha, dev) \
	netdev_hw_addr_list_for_each(ha, &(dev)->mc)

struct ddk_net_device
{
	struct list_head	dev_list;
	
	const struct ddk_net_device_ops *netdev_ops;
	const struct ddk_net_device_header_ops *header_ops;

	unsigned int		flags;							/* interface flags (a la BSD)	*/
	unsigned int		priv_flags;						/* Like 'flags' but invisible to userspace */

	/* currently active device features */
	netdev_features_t	features;
	/* user-changeable features */
	netdev_features_t	hw_features;

	int mtu;
	unsigned short		type;							/* interface hardware type，参见ether_setup	*/
	unsigned short		hard_header_len;				/* hardware hdr length	*/
	
	/* Interface address info. */
	unsigned char		perm_addr[MAX_ADDR_LEN]; 
	unsigned char		addr_assign_type;
	unsigned char		addr_len;						/* 硬件地址长度，不同的网卡不同的长度，参见ether_setup*/
	
	struct netdev_hw_addr_list	uc;	/* Unicast mac addresses */
	struct netdev_hw_addr_list	mc;	/* Multicast mac addresses */

	unsigned char		dev_addr[MAX_ADDR_LEN];			/*dev_addr_init*/

	unsigned char		broadcast[MAX_ADDR_LEN];
	unsigned long		tx_queue_len;					/* Max frames per queue allowed */
};

#define	NETDEV_ALIGN		32
static inline void *netdev_priv(const struct ddk_net_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct ddk_net_device)+2*12, NETDEV_ALIGN);
}


DLLEXPORT int netdev_register(struct ddk_net_device *dev);

DLLEXPORT int netdev_open(struct ddk_net_device *dev);

//int netdev_info(const struct ddk_net_device *dev, const char *format, ...);
#include <ddk/debug.h>
#define netdev_info(dev, fmt, arg...) //printk("netdev_info："fmt, ##arg)
#define netdev_err(dev, fmt, arg...) printk("netdev_err:"fmt, ##arg)
#define register_netdev netdev_register

#endif
/** @} */
