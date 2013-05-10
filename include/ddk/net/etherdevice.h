/**
*  @defgroup net_etherdevice
*  @ingroup DDK
*
*  �����˾������豸��������
*  @{
*/
#ifndef _DDK_NET_ETHER_DEVICE_H_
#define _DDK_NET_ETHER_DEVICE_H_

#include <ddk/net/netdevice.h>

extern struct net_device *alloc_etherdev_mqs(int sizeof_priv, unsigned int txqs,
											 unsigned int rxqs);


/* Hardware interface define of ethernet device  */
/*
 *	IEEE 802.3 Ethernet magic constants.  The frame sizes omit the preamble
 *	and FCS/CRC (frame check sequence).
 */

#define ETH_ALEN	6		/* Octets in one ethernet addr	 */
#define ETH_HLEN	14		/* Total octets in header.	 */
#define ETH_ZLEN	60		/* Min. octets in frame sans FCS */
#define ETH_DATA_LEN	1500		/* Max. octets in payload	 */
#define ETH_FRAME_LEN	1514		/* Max. octets in frame sans FCS */
#define ETH_FCS_LEN	4		/* Octets in the FCS		 */

/* ����arp����������һ�����������豸������ */
#define ARPHRD_ETHER 	1		/* Ethernet 10Mbps		*/

#endif
/** @} */
