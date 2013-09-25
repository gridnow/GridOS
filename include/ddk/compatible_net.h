/*
	OLD driver need this
	Net related
*/
#ifndef DDK_COMPATIBLE_INTERFACE_NET
#define DDK_COMPATIBLE_INTERFACE_NET

#include <ddk/net/rt.h>

#define alloc_etherdev(sizeof_priv) alloc_etherdev_mq(sizeof_priv, 1)
#define alloc_etherdev_mq(sizeof_priv, count) alloc_etherdev_mqs(sizeof_priv, count, count)

#define sk_buff ddk_net_buf
#define skb_shinfo(skb) (&(skb->desc))
static inline struct iphdr *ip_hdr(struct sk_buff *skb)
{
	return &skb->network_hdr.network_hdr_ip;
}

static inline unsigned int skb_headlen(const struct sk_buff *skb)
{
	return skb->len - skb->data_len;
}

static inline void netdev_reset_queue(struct ddk_net_device *dev_queue)
{

}
static inline void netif_stop_queue(struct ddk_net_device *dev)
{

}

static inline struct sk_buff *netdev_alloc_skb_ip_align(struct ddk_net_device *dev,
														unsigned int length)
{
	//TODO: to extend room, we need the buffer to map to hardware RX
	return netdev_nb_alloc(dev);
}

#define CHECKSUM_NONE 0
#define CHECKSUM_UNNECESSARY 1
#define CHECKSUM_COMPLETE 2
#define CHECKSUM_PARTIAL 3

#endif

