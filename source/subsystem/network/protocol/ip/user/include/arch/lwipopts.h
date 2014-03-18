#define LWIP_ARP 1
#define LWIP_TCP 1
#define LWIP_UDP 1

/* �����ڶ����ģʽ�� */
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

/* TCP ,536 ֻ�ܴﵽ 199M, 1460 �ﵽ400MB ��10 ��a.out��*/
#define TCP_MSS 1460

/* ��ǰ����ʹ��HINT */
#define LWIP_NETIF_HWADDRHINT 1

/* Called when tcp has event */
#define LWIP_CALLBACK_API		1

/* If define it, we use host, or use the simple ones */
//#define LWIP_PREFIX_BYTEORDER_FUNCS	1

/* Debug options */
#define PBUF_DEBUG					LWIP_DBG_ON
#define LWIP_DEBUG
#define ICMP_DEBUG					LWIP_DBG_ON
#define IP_REASS_DEBUG				LWIP_DBG_ON
#define IP_DEBUG					LWIP_DBG_ON

#define UDP_DEBUG					LWIP_DBG_ON

#define NETIF_DEBUG					LWIP_DBG_ON
#define ETHARP_DEBUG				LWIP_DBG_ON
#define INET_DEBUG					LWIP_DBG_ON
#define ETHARP_DEBUG_RAND			LWIP_DBG_ON
#define TCP_RST_DEBUG				LWIP_DBG_ON
#define TCP_DELAY_ACK_DEBUG			LWIP_DBG_ON
#define TCP_DEBUG					LWIP_DBG_ON
#define TCP_RTO_DEBUG				LWIP_DBG_ON	/* TCP retransmit */
#define TCP_CWND_DEBUG				LWIP_DBG_ON /* TCP congestion window */
#define TCP_INPUT_DEBUG				LWIP_DBG_ON
#define TCP_OUTPUT_DEBUG			LWIP_DBG_ON
#define TCP_FR_DEBUG				LWIP_DBG_ON	/* TCP for fast retransmit */
#define TCP_QLEN_DEBUG				LWIP_DBG_ON /* Enable debugging for TCP queue lengths. */
#define TCP_WND_DEBUG				LWIP_DBG_ON /* Enable debugging in tcp_in.c for window updating. */

#define LWIP_DBG_TYPES_ON			(LWIP_DBG_ON|LWIP_DBG_TRACE|LWIP_DBG_STATE|LWIP_DBG_FRESH|LWIP_DBG_HALT)
#define LWIP_DBG_MIN_LEVEL			0


#define MEM_ALIGNMENT 4

/* Do not use the system mode */
#define NO_SYS						1

/* Do not use its socket */
#define LWIP_NETCONN				0
#define LWIP_SOCKET					0

/* Now ip raw input is supported */
#define LWIP_RAW					0

/* Use system memory allocator */
#define MEM_LIBC_MALLOC				1