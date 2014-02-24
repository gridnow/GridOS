/**
	MyTcpIp

	Ethernet ARP Protocol

	Sihai
*/

#ifndef MYIP_ETHER_ARP_H
#define MYIP_ETHER_ARP_H

#include <string.h>
#include "core/def.h"
#include "core/ip_addr.h"

#ifndef ETHARP_HWADDR_LEN
#define ETHARP_HWADDR_LEN     6
#endif

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
struct eth_addr {
	PACK_STRUCT_FIELD(u8_t addr[ETHARP_HWADDR_LEN]);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
/** Ethernet header */
struct eth_hdr {
#if ETH_PAD_SIZE
	PACK_STRUCT_FIELD(u8_t padding[ETH_PAD_SIZE]);
#endif
	PACK_STRUCT_FIELD(struct eth_addr dest);
	PACK_STRUCT_FIELD(struct eth_addr src);
	PACK_STRUCT_FIELD(u16_t type);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define SIZEOF_ETH_HDR (14 + ETH_PAD_SIZE)

#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/bpstruct.h"
#endif
PACK_STRUCT_BEGIN
/** the ARP message, see RFC 826 ("Packet format") */
struct etharp_hdr {
  PACK_STRUCT_FIELD(u16_t hwtype);
  PACK_STRUCT_FIELD(u16_t proto);
  PACK_STRUCT_FIELD(u8_t  hwlen);
  PACK_STRUCT_FIELD(u8_t  protolen);
  PACK_STRUCT_FIELD(u16_t opcode);
  PACK_STRUCT_FIELD(struct eth_addr shwaddr);
  PACK_STRUCT_FIELD(struct ip_addr2 sipaddr);
  PACK_STRUCT_FIELD(struct eth_addr dhwaddr);
  PACK_STRUCT_FIELD(struct ip_addr2 dipaddr);
} PACK_STRUCT_STRUCT;
PACK_STRUCT_END
#ifdef PACK_STRUCT_USE_INCLUDES
#  include "arch/epstruct.h"
#endif

#define SIZEOF_ETHARP_HDR 28
#define SIZEOF_ETHARP_PACKET (SIZEOF_ETH_HDR + SIZEOF_ETHARP_HDR)

/** 5 seconds period */
#define ARP_TMR_INTERVAL 5000

#define ETHTYPE_ARP       0x0806U
#define ETHTYPE_IP        0x0800U
#define ETHTYPE_VLAN      0x8100U
#define ETHTYPE_PPPOEDISC 0x8863U  /* PPP Over Ethernet Discovery Stage */
#define ETHTYPE_PPPOE     0x8864U  /* PPP Over Ethernet Session Stage */

/** MEMCPY-like macro to copy to/from struct eth_addr's that are local variables
 * or known to be 32-bit aligned within the protocol header. */
#ifndef ETHADDR32_COPY
#define ETHADDR32_COPY(src, dst)  SMEMCPY(src, dst, ETHARP_HWADDR_LEN)
#endif

/** MEMCPY-like macro to copy to/from struct eth_addr's that are no local
 * variables and known to be 16-bit aligned within the protocol header. */
#ifndef ETHADDR16_COPY
#define ETHADDR16_COPY(src, dst)  SMEMCPY(src, dst, ETHARP_HWADDR_LEN)
#endif

/** ARP message types (opcodes) */
#define ARP_REQUEST 1
#define ARP_REPLY   2

/** Define this to 1 and define LWIP_ARP_FILTER_NETIF_FN(pbuf, netif, type)
 * to a filter function that returns the correct netif when using multiple
 * netifs on one hardware interface where the netif's low-level receive
 * routine cannot decide for the correct netif (e.g. when mapping multiple
 * IP addresses to one hardware interface).
 */
#ifndef LWIP_ARP_FILTER_NETIF
#define LWIP_ARP_FILTER_NETIF 0
#endif

#if ARP_QUEUEING
/** struct for queueing outgoing packets for unknown address
  * defined here to be accessed by memp.h
  */
struct etharp_q_entry {
  struct etharp_q_entry *next;
  struct pbuf *p;
};
#endif /* ARP_QUEUEING */

/** the time an ARP entry stays valid after its last update,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (240 * 5) seconds = 20 minutes.
 */
#define ARP_MAXAGE              240
/** Re-request a used ARP entry 1 minute before it would expire to prevent
 *  breaking a steadily used connection because the ARP entry timed out. */
#define ARP_AGE_REREQUEST_USED  (ARP_MAXAGE - 12)

/** the time an ARP entry stays pending after first request,
 *  for ARP_TMR_INTERVAL = 5000, this is
 *  (2 * 5) seconds = 10 seconds.
 * 
 *  @internal Keep this number at least 2, otherwise it might
 *  run out instantly if the timeout occurs directly after a request.
 */
#define ARP_MAXPENDING 2

#define HWTYPE_ETHERNET 1

enum etharp_state {
	ETHARP_STATE_EMPTY = 0,
	ETHARP_STATE_PENDING,
	ETHARP_STATE_STABLE,
	ETHARP_STATE_STABLE_REREQUESTING
#if ETHARP_SUPPORT_STATIC_ENTRIES
	,ETHARP_STATE_STATIC
#endif /* ETHARP_SUPPORT_STATIC_ENTRIES */
};

struct etharp_entry {
#if ARP_QUEUEING
	/** Pointer to queue of pending outgoing packets on this ARP entry. */
	struct etharp_q_entry *q;
#else /* ARP_QUEUEING */
	/** Pointer to a single pending outgoing packet on this ARP entry. */
	struct pbuf *q;
#endif /* ARP_QUEUEING */
	ip_addr_t ipaddr;
	struct netif *netif;
	struct eth_addr ethaddr;
	u8_t state;
	u8_t ctime;
};

/** Try hard to create a new entry - we want the IP address to appear in
    the cache (even if this means removing an active entry or so). */
#define ETHARP_FLAG_TRY_HARD     1
#define ETHARP_FLAG_FIND_ONLY    2
struct eth_private
{
	struct etharp_entry arp_table[ARP_TABLE_SIZE];
};

static inline struct eth_private *netif_to_ethernet(struct netif *netif)
{
	struct eth_private *p = netif_get_private(netif);
	return p;
}

/**
	@brief Send an ARP request packet asking for ipaddr.

	仅仅是发送ARP请求，与etharp_query（以太网卡出口)不一样
*/
err_t etharp_request(struct netif *netif, ip_addr_t *ipaddr);

/**
	@brief The package from ethernet is first going to ARP layer
*/
err_t ethernet_input(struct pbuf *p, struct netif *netif);

/**
 * @brief Send an ARP request for the given IP address and/or queue a packet.
 *
 * If the IP address was not yet in the cache, a pending ARP cache entry
 * is added and an ARP request is sent for the given address. The packet
 * is queued on this entry.
 *
 * If the IP address was already pending in the cache, a new ARP request
 * is sent for the given address. The packet is queued on this entry.
 *
 * If the IP address was already stable in the cache, and a packet is
 * given, it is directly sent and no ARP request is sent out. 
 * 
 * If the IP address was already stable in the cache, and no packet is
 * given, an ARP request is sent out.
 * 
 * @param[in] netif The lwIP network interface on which ipaddr must be queried for.
 * @param[in] ipaddr The IP address to be resolved.
 * @param[in] q If non-NULL, a pbuf that must be delivered to the IP address q is not freed by this function.
 *
 * @note q must only be ONE packet, not a packet queue!
 *
 * @return
 * - ERR_BUF Could not make room for Ethernet header.
 * - ERR_MEM Hardware address unknown, and no more ARP entries available
 *   to query for address or queue the packet.
 * - ERR_MEM Could not queue packet due to memory shortage.
 * - ERR_RTE No route to destination (no gateway to external networks).
 * - ERR_ARG Non-unicast address given, those will not appear in ARP cache.
 *
 */
err_t etharp_query(struct netif *netif, ip_addr_t *ipaddr, struct pbuf *q);

/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
err_t etharp_output(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr);

#define eth_addr_cmp(addr1, addr2) (memcmp((addr1)->addr, (addr2)->addr, ETHARP_HWADDR_LEN) == 0)

#endif