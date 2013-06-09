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
#include "features.h"

#define net_device ddk_net_device
#define header_ops ddk_net_device_header_ops
struct sk_buff;
struct ddk_net_device;

struct ddk_net_device_header_ops {
	int	(*create) (struct sk_buff *skb, struct ddk_net_device *dev,
				   unsigned short type, const void *daddr,
				   const void *saddr, unsigned int len);
	int	(*parse)(const struct sk_buff *skb, unsigned char *haddr);
	int	(*rebuild)(struct sk_buff *skb);
};

enum netdev_tx {
	NETDEV_TX_OK	 = 0x00,
	NETDEV_TX_BUSY	 = 0x10,
	NETDEV_TX_LOCKED = 0x20,
};
typedef enum netdev_tx netdev_tx_t;

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
	netdev_tx_t					(*start_xmit) (struct sk_buff *skb, struct ddk_net_device *dev);
	void						(*set_rx_mode)(struct ddk_net_device *dev);
	int							(*set_mac_address)(struct ddk_net_device *dev, void *addr);
	struct net_device_stats*	(*get_stats)(struct ddk_net_device *dev);
	int							(*change_mtu)(struct ddk_net_device *dev, int new_mtu);
	void						(*tx_timeout) (struct ddk_net_device *dev);
	int							(*set_features)(struct ddk_net_device *dev, netdev_features_t features);
};

#define MAX_ADDR_LEN	32		/* Largest hardware address length */
struct ddk_net_device
{
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
	
	unsigned char		dev_addr[MAX_ADDR_LEN];			/*dev_addr_init*/

	unsigned char		broadcast[MAX_ADDR_LEN];
	unsigned long		tx_queue_len;					/* Max frames per queue allowed */
};

/*
 * 调试信息设置
 */
enum {
	NETIF_MSG_DRV		= 0x0001,
	NETIF_MSG_PROBE		= 0x0002,
	NETIF_MSG_LINK		= 0x0004,
	NETIF_MSG_TIMER		= 0x0008,
	NETIF_MSG_IFDOWN	= 0x0010,
	NETIF_MSG_IFUP		= 0x0020,
	NETIF_MSG_RX_ERR	= 0x0040,
	NETIF_MSG_TX_ERR	= 0x0080,
	NETIF_MSG_TX_QUEUED	= 0x0100,
	NETIF_MSG_INTR		= 0x0200,
	NETIF_MSG_TX_DONE	= 0x0400,
	NETIF_MSG_RX_STATUS	= 0x0800,
	NETIF_MSG_PKTDATA	= 0x1000,
	NETIF_MSG_HW		= 0x2000,
	NETIF_MSG_WOL		= 0x4000,
};

#define	NETDEV_ALIGN		32
static inline void *netdev_priv(const struct ddk_net_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct ddk_net_device), NETDEV_ALIGN);
}

static inline void netdev_reset_queue(struct ddk_net_device *dev_queue)
{
	
}

static inline int register_netdev(struct ddk_net_device *dev)
{

}

//int netdev_info(const struct ddk_net_device *dev, const char *format, ...);
#include <ddk/debug.h>
#define netdev_info(dev, fmt, arg...) printk("netdev_info："fmt, ##arg)

#endif
/** @} */
