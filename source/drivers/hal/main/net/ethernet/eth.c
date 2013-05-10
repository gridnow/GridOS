#include <ddk/types.h>
#include <ddk/string.h>

#include <ddk/net/netdevice.h>
#include <ddk/net/etherdevice.h>
#include <ddk/net/if.h>

const struct header_ops eth_header_ops /*____cacheline_aligned*/ = {
	//TODO
};

void ether_setup(struct net_device *dev)
{
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
 *
 * Constructs a new net device, complete with a private data area of
 * size (sizeof_priv).  A 32-byte (not bit) alignment is enforced for
 * this private data area.
 */

struct net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs,
									  unsigned int rxqs)
{
	return alloc_netdev_mqs(sizeof_priv, "eth%d", ether_setup, txqs, rxqs);
}
