/**
*  @defgroup net_rt
*  @ingroup DDK
*
*  定义了网络开发时的公共定义
*  @{
*/

#ifndef _DDK_NET_DEFINE_H_
#define _DDK_NET_DEFINE_H_

/*
	发送状态码
*/
enum netdev_tx {
	NETDEV_TX_OK	 = 0x00,
	NETDEV_TX_BUSY	 = 0x10,
	NETDEV_TX_LOCKED = 0x20,
};
typedef enum netdev_tx netdev_tx_t;

/*
	调试信息设置
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

#endif
/** @} */
