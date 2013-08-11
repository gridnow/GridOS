#include <ddk/types.h>
#include <ddk/string.h>

#include <ddk/net/netdevice.h>
#include <ddk/net/etherdevice.h>
#include <ddk/net/if.h>

#include "../core/internal.h"

const struct header_ops eth_header_ops /*____cacheline_aligned*/ = {
	//TODO
};

void ether_setup(net_device_t *netdev)
{
	struct ddk_net_device *dev = (struct ddk_net_device*)netdev;
	
	dev->header_ops		= &eth_header_ops;
	dev->type		= ARPHRD_ETHER;
	dev->hard_header_len 	= ETH_HLEN;
	dev->mtu		= ETH_DATA_LEN;
	dev->addr_len		= ETH_ALEN;
	dev->tx_queue_len	= 1000;	/* Ethernet wants good queues */
	dev->flags		= IFF_BROADCAST|IFF_MULTICAST;
	dev->priv_flags		|= IFF_TX_SKB_SHARING;
	
	memset(dev->broadcast, 0xFF, ETH_ALEN);
}

/**
 * alloc_etherdev_mqs - Allocates and sets up an Ethernet device
*/
struct ddk_net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs,
									  unsigned int rxqs)
{
	return (struct ddk_net_device *)alloc_netdev_mqs(sizeof_priv, "eth%d", ether_setup, txqs, rxqs);
}
