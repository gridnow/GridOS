
/**
*  @defgroup net_device
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

#define MAX_ADDR_LEN	32		/* Largest hardware address length */


struct ddk_net_device_header_ops {
	int	(*create) (struct sk_buff *skb, struct net_device *dev,
				   unsigned short type, const void *daddr,
				   const void *saddr, unsigned int len);
	int	(*parse)(const struct sk_buff *skb, unsigned char *haddr);
	int	(*rebuild)(struct sk_buff *skb);
	//	int	(*cache)(const struct neighbour *neigh, struct hh_cache *hh, __be16 type);
	//	void	(*cache_update)(struct hh_cache *hh,
	//							const struct net_device *dev,
	//							const unsigned char *haddr);
};

struct ddk_net_device
{
	/* Hardware header description */
	const struct ddk_net_device_header_ops *header_ops;

	unsigned int		flags;	/* interface flags (a la BSD)	*/
	unsigned int		priv_flags; /* Like 'flags' but invisible to userspace */

	/* currently active device features */
	netdev_features_t	features;
	/* user-changeable features */
	netdev_features_t	hw_features;

	int mtu;
	unsigned short		type;	/* interface hardware type	*/
	unsigned short		hard_header_len;	/* hardware hdr length	*/
	
	/* Interface address info. */
	
	unsigned char		addr_assign_type; /* hw address assignment type */
	unsigned char		addr_len;	/* hardware address length	*/
	
	unsigned char		broadcast[MAX_ADDR_LEN];	/* hw bcast add	*/
	unsigned long		tx_queue_len;	/* Max frames per queue allowed */
	
	
};

#define	NETDEV_ALIGN		32

/*
 * Network interface message level settings
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

static inline void *netdev_priv(const struct net_device *dev)
{
	return (char *)dev + ALIGN(sizeof(struct net_device), NETDEV_ALIGN);
}

#endif
/** @} */
