/**
*  @defgroup net_if
*  @ingroup DDK
*
*  定义了网络媒体介质信息
*  @{
*/

#ifndef _DDK_NET_IF_H_
#define _DDK_NET_IF_H_

/* Standard interface flags (netdevice->flags). */
#define	IFF_UP		0x1		/* interface is up		*/
#define	IFF_BROADCAST	0x2		/* broadcast address valid	*/

#define IFF_MULTICAST	0x1000		/* Supports multicast		*/

#define IFF_TX_SKB_SHARING	0x10000	/* The interface supports sharing skbs on transmit */

#endif
/** @} */
