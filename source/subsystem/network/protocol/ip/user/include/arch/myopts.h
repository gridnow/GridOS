#define LWIP_ARP 1
#define LWIP_TCP 1
#define LWIP_UDP 1

/* 我们在多队列模式下 */
#define ARCH_USE_FDIR 1

/* IP check is done by HW if we are not in testing */
#ifndef _MSC_VER
#define CHECKSUM_CHECK_IP 0
#define CHECKSUM_CHECK_TCP 0
#define CHECKSUM_CHECK_UDP 0
#define CHECKSUM_GEN_IP 0
#define CHECKSUM_GEN_UDP 0
#define CHECKSUM_GEN_TCP 0
#endif

/* TCP ,536 只能达到 199M, 1460 达到400MB （10 个a.out）*/
#define TCP_MSS 1460

/* 当前必须使用HINT */
#define LWIP_NETIF_HWADDRHINT 1

/* Called when tcp has event */
#define LWIP_CALLBACK_API		1

/* htons etc conflict with host OS, so use host */
#define LWIP_PREFIX_BYTEORDER_FUNCS	1

/* Debug options */
#define PBUF_DEBUG					LWIP_DBG_OFF
#define LWIP_DEBUG
#define ICMP_DEBUG					LWIP_DBG_OFF
#define IP_REASS_DEBUG				LWIP_DBG_OFF
#define IP_DEBUG					LWIP_DBG_OFF

#define UDP_DEBUG					LWIP_DBG_OFF

#define NETIF_DEBUG					LWIP_DBG_OFF
#define ETHARP_DEBUG				LWIP_DBG_OFF
#define INET_DEBUG					LWIP_DBG_OFF
#define ETHARP_DEBUG_RAND			LWIP_DBG_ON
#define TCP_RST_DEBUG				LWIP_DBG_ON
#define TCP_DELAY_ACK_DEBUG			LWIP_DBG_ON
#define TCP_DEBUG					LWIP_DBG_OFF
#define TCP_RTO_DEBUG				LWIP_DBG_OFF	/* TCP retransmit */
#define TCP_CWND_DEBUG				LWIP_DBG_OFF /* TCP congestion window */
#define TCP_INPUT_DEBUG				LWIP_DBG_OFF
#define TCP_OUTPUT_DEBUG			LWIP_DBG_OFF
#define TCP_FR_DEBUG				LWIP_DBG_ON	/* TCP for fast retransmit */
#define TCP_QLEN_DEBUG				LWIP_DBG_ON /* Enable debugging for TCP queue lengths. */
#define TCP_WND_DEBUG				LWIP_DBG_OFF /* Enable debugging in tcp_in.c for window updating. */

#define LWIP_DBG_TYPES_ON			(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_MIN_LEVEL			0


#define MEM_ALIGNMENT 4

