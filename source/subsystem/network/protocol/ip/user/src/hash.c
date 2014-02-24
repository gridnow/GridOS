/**
	Hash匹配：port　＆ IP

	PCB 搜索用到本模块

*/
#include "core/ip_addr.h"
#include "core/tcp.h"
#include "core/tcp_impl.h"

#include "core/hash.h"
#include "core/hlist_nulls.h"

/* jhash.h: Jenkins hash support.
 *
 * Copyright (C) 2006. Bob Jenkins (bob_jenkins@burtleburtle.net)
 *
 * http://burtleburtle.net/bob/hash/
 *
 * These are the credits from Bob's sources:
 *
 * lookup3.c, by Bob Jenkins, May 2006, Public Domain.
 *
 * These are functions for producing 32-bit hashes for hash table lookup.
 * hashword(), hashlittle(), hashlittle2(), hashbig(), mix(), and final()
 * are externally useful functions.  Routines to test the hash are included
 * if SELF_TEST is defined.  You can use this free for any purpose.  It's in
 * the public domain.  It has no warranty.
 *
 */
#include "core/def.h"

struct __una_u32 { u32 x; } __packed;
static inline u32 __get_unaligned_cpu32(const void *p)
{
	const struct __una_u32 *ptr = (const struct __una_u32 *)p;
	return ptr->x;
}

/**
 * rol32 - rotate a 32-bit value left
 * @word: value to rotate
 * @shift: bits to roll
 */
static inline __u32 rol32(__u32 word, unsigned int shift)
{
	return (word << shift) | (word >> (32 - shift));
}

/* Best hash sizes are of power of two */
#define jhash_size(n)   ((u32)1<<(n))
/* Mask the hash value, i.e (value & jhash_mask(n)) instead of (value % n) */
#define jhash_mask(n)   (jhash_size(n)-1)

/* __jhash_mix -- mix 3 32-bit values reversibly. */
#define __jhash_mix(a, b, c)			\
{						\
	a -= c;  a ^= rol32(c, 4);  c += b;	\
	b -= a;  b ^= rol32(a, 6);  a += c;	\
	c -= b;  c ^= rol32(b, 8);  b += a;	\
	a -= c;  a ^= rol32(c, 16); c += b;	\
	b -= a;  b ^= rol32(a, 19); a += c;	\
	c -= b;  c ^= rol32(b, 4);  b += a;	\
}

/* __jhash_final - final mixing of 3 32-bit values (a,b,c) into c */
#define __jhash_final(a, b, c)			\
{						\
	c ^= b; c -= rol32(b, 14);		\
	a ^= c; a -= rol32(c, 11);		\
	b ^= a; b -= rol32(a, 25);		\
	c ^= b; c -= rol32(b, 16);		\
	a ^= c; a -= rol32(c, 4);		\
	b ^= a; b -= rol32(a, 14);		\
	c ^= b; c -= rol32(b, 24);		\
}

/* An arbitrary initial parameter */
#define JHASH_INITVAL		0xdeadbeef

/* jhash - hash an arbitrary key
 * @k: sequence of bytes as key
 * @length: the length of the key
 * @initval: the previous hash, or an arbitray value
 *
 * The generic version, hashes an arbitrary sequence of bytes.
 * No alignment or length assumptions are made about the input key.
 *
 * Returns the hash value of the key. The result depends on endianness.
 */
static inline u32 jhash(const void *key, u32 length, u32 initval)
{
	u32 a, b, c;
	const u8 *k = key;

	/* Set up the internal state */
	a = b = c = JHASH_INITVAL + length + initval;

	/* All but the last block: affect some 32 bits of (a,b,c) */
	while (length > 12) {
		a += __get_unaligned_cpu32(k);
		b += __get_unaligned_cpu32(k + 4);
		c += __get_unaligned_cpu32(k + 8);
		__jhash_mix(a, b, c);
		length -= 12;
		k += 12;
	}
	/* Last block: affect all 32 bits of (c) */
	/* All the case statements fall through */
	switch (length) {
	case 12: c += (u32)k[11]<<24;
	case 11: c += (u32)k[10]<<16;
	case 10: c += (u32)k[9]<<8;
	case 9:  c += k[8];
	case 8:  b += (u32)k[7]<<24;
	case 7:  b += (u32)k[6]<<16;
	case 6:  b += (u32)k[5]<<8;
	case 5:  b += k[4];
	case 4:  a += (u32)k[3]<<24;
	case 3:  a += (u32)k[2]<<16;
	case 2:  a += (u32)k[1]<<8;
	case 1:  a += k[0];
		 __jhash_final(a, b, c);
	case 0: /* Nothing left to add */
		break;
	}

	return c;
}

/* jhash2 - hash an array of u32's
 * @k: the key which must be an array of u32's
 * @length: the number of u32's in the key
 * @initval: the previous hash, or an arbitray value
 *
 * Returns the hash value of the key.
 */
static inline u32 jhash2(const u32 *k, u32 length, u32 initval)
{
	u32 a, b, c;

	/* Set up the internal state */
	a = b = c = JHASH_INITVAL + (length<<2) + initval;

	/* Handle most of the key */
	while (length > 3) {
		a += k[0];
		b += k[1];
		c += k[2];
		__jhash_mix(a, b, c);
		length -= 3;
		k += 3;
	}

	/* Handle the last 3 u32's: all the case statements fall through */
	switch (length) {
	case 3: c += k[2];
	case 2: b += k[1];
	case 1: a += k[0];
		__jhash_final(a, b, c);
	case 0:	/* Nothing left to add */
		break;
	}

	return c;
}


/* jhash_3words - hash exactly 3, 2 or 1 word(s) */
static inline u32 jhash_3words(u32 a, u32 b, u32 c, u32 initval)
{
	a += JHASH_INITVAL;
	b += JHASH_INITVAL;
	c += initval;

	__jhash_final(a, b, c);

	return c;
}

static inline u32 jhash_2words(u32 a, u32 b, u32 initval)
{
	return jhash_3words(a, b, 0, initval);
}

static inline u32 jhash_1word(u32 a, u32 initval)
{
	return jhash_3words(a, 0, 0, initval);
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

#define LISTENING_NULLS_BASE (1U << 29)
/* This is for listening sockets, thus all sockets which possess wildcards. */
#define INET_LHTABLE_SIZE	32	/* Yes, really, this is all you need. */

#define L1_CACHE_SHIFT 6
#define inet_ehash_secret 0 /* build_ehash_secret 把它初始化成一个随机值 */
static inline unsigned int net_hash_mix(void *net)
{
	/*
	 * shift this right to eliminate bits, that are
	 * always zeroed
	 */
	return (unsigned)(((unsigned long)(unsigned long long)net) >> L1_CACHE_SHIFT);
}

/* Established pcb */
struct inet_ehash_bucket {
	struct hlist_nulls_head chain;
	struct hlist_nulls_head twchain;
};

/* Listening pcb */
struct inet_listen_hashbucket {	
	struct hlist_nulls_head	head;
};

/* The total list */
struct inet_hashinfo {
	struct inet_ehash_bucket	*ehash;
	unsigned int			ehash_mask;
	struct inet_listen_hashbucket	listening_hash[INET_LHTABLE_SIZE];
};

static inline unsigned int inet_ehashfn(void *net,
										const __be32 laddr, const __u16 lport,
										const __be32 faddr, const __be16 fport)
{
	return jhash_3words((__force __u32) laddr,
		(__force __u32) faddr,
		((__u32) lport) << 16 | (__force __u32)fport,
		inet_ehash_secret + net_hash_mix(net));
}

static inline int inet_lhashfn(void *net, const unsigned short num)
{
	return (num + net_hash_mix(net)) & (INET_LHTABLE_SIZE - 1);
}

static inline struct inet_ehash_bucket *inet_ehash_bucket(struct inet_hashinfo *hashinfo,
	unsigned int hash)
{
	return &hashinfo->ehash[hash & hashinfo->ehash_mask];
}

static struct tcp_pcb *lookup_common_v4(struct hlist_nulls_head *chain_head, unsigned int hash, u16_t rport, u16_t lport, u32 rip, u32 lip)
{
	struct tcp_pcb *pcb;
	int found = 0;
	const struct hlist_nulls_node *node;

	hlist_nulls_for_each_entry(pcb, node, chain_head, pcb_nulls_node, struct tcp_pcb) {
		if (pcb->pcb_hash != hash)
			continue;
		
		/* Really is the pcb we want? */
		if (pcb->remote_port == rport &&
			pcb->local_port == lport &&
			ip_addr_cmp(&(pcb->remote_ip), (struct ip_addr*)&rip) &&
			ip_addr_cmp(&(pcb->local_ip), (struct ip_addr*)&lip)) {
				found = 1;
				break;
		}
	}

	if (found == 0)
		return NULL;
	return pcb;
}

static void add_common_v4(struct tcp_pcb *who, struct hlist_nulls_head *list, unsigned int hash, int type)
{
	hlist_nulls_add_head(&who->pcb_nulls_node, list);

	who->pcb_hash = hash;	
	who->hash_list_in = type;
}

/************************************************************************/
/* External methods                                                     */
/************************************************************************/
/**
	@brief Create the hash table for TCP v4
*/
void tcp_v4_hash_create(struct inet_hashinfo **hash)
{
	int i;
	struct inet_hashinfo *h = calloc(1, sizeof(*h));

	/* Linux 一般用4MB */
#define HAS_SIZE (0x400000 / sizeof(*h->ehash))

	*hash = NULL;
	if (!h)
		goto err;
	h->ehash = calloc(HAS_SIZE, sizeof(*h->ehash));
	if (!h->ehash)
		goto err;
	
	h->ehash_mask = HAS_SIZE - 1;
	for (i = 0; i <= (int)h->ehash_mask; i++) {
		INIT_HLIST_NULLS_HEAD(&h->ehash[i].chain, i);
		INIT_HLIST_NULLS_HEAD(&h->ehash[i].twchain, i);
	}
	for (i = 0; i < INET_LHTABLE_SIZE; i++) {
		INIT_HLIST_NULLS_HEAD(&h->listening_hash[i].head,
			i + LISTENING_NULLS_BASE);
	}
	*hash = h;
	return;

err:
	if (h)
	{
		if (h->ehash)
			free(h->ehash);
		free(h);
	}
}

/**
	@brief Add an active TCP's PCB to hash table
*/
void tcp_v4_hash_add_active_pcb(struct netif *where, struct tcp_pcb *who, u16_t rport, u16_t lport,
						u32 rip, u32 lip)
{
	struct inet_hashinfo *hashinfo = where->tcp_hash;
	struct inet_ehash_bucket *head;
	struct hlist_nulls_head *list;
	unsigned int hash;

	/* Sanity check if already in it */
	if (who->hash_list_in != HASH_LIST_UNKNOWN)
		return ;

	/* A bit like __inet_hash_nolisten */	
	hash = inet_ehashfn(where, lip, lport, rip, rport);	
	head = inet_ehash_bucket(hashinfo, hash);
	list = &head->chain;
	add_common_v4(who, list, hash, HASH_LIST_ACTIVE);
}

/**
	@brief Add a TW TCP's PCB to hash table
*/
void tcp_v4_hash_add_tw_pcb(struct netif *where, struct tcp_pcb *who, u16_t rport, u16_t lport,
							u32 rip, u32 lip)
{
	struct inet_hashinfo *hashinfo = where->tcp_hash;
	struct inet_ehash_bucket *head;
	struct hlist_nulls_head *list;
	unsigned int hash;

	/* Sanity check if already in it */
	if (who->hash_list_in != HASH_LIST_UNKNOWN)
		return ;

	/* A bit like __inet_hash_nolisten */	
	hash = inet_ehashfn(where, lip, lport, rip, rport);	
	head = inet_ehash_bucket(hashinfo, hash);
	list = &head->twchain;
	add_common_v4(who, list, hash, HASH_LIST_TW);
}

/**
	@brief Find the pcb from Active hash list
*/
struct tcp_pcb *tcp_v4_hash_lookup_active_pcb(struct netif *where, u16_t rport, u16_t lport, u32 rip, u32 lip)
{ 
	struct inet_hashinfo *hinfo = where->tcp_hash;	
	unsigned int hash = inet_ehashfn(where, lip, lport, rip, rport);
	unsigned int slot = hash & hinfo->ehash_mask;
	struct inet_ehash_bucket *head = &hinfo->ehash[slot];

	return lookup_common_v4(&head->chain, hash, rport, lport, rip, lip);
}

/**
	@brief Find the pcb from TW hash list
*/
struct tcp_pcb *tcp_v4_hash_lookup_tw_pcb(struct netif *where, u16_t rport, u16_t lport, u32 rip, u32 lip)
{ 
	struct inet_hashinfo *hinfo = where->tcp_hash;	
	unsigned int hash = inet_ehashfn(where, lip, lport, rip, rport);
	unsigned int slot = hash & hinfo->ehash_mask;
	struct inet_ehash_bucket *head = &hinfo->ehash[slot];

	return lookup_common_v4(&head->twchain, hash, rport, lport, rip, lip);
}

/**
	@brief Find the pcb from Listen hash list
*/
struct tcp_pcb_listen *tcp_v4_hash_lookup_listen_pcb(struct netif *where, unsigned int short local_port, u32 local_ip)
{
	struct tcp_pcb_listen *pcb;
	struct inet_hashinfo *hinfo = where->tcp_hash;	

	struct hlist_nulls_node *node;
	unsigned int hash = inet_lhashfn(where, local_port);
	struct inet_listen_hashbucket *ilb = &hinfo->listening_hash[hash];

	hlist_nulls_for_each_entry(pcb, node, &ilb->head, pcb_nulls_node, struct tcp_pcb_listen) {
		if (pcb->local_port == local_port /*&& pcb->local_ip.addr == local_ip*//*TODO:The ip may be "any"*/)
			return pcb;
	}

	return NULL;
}

/**
	@brief Add a PCB in listen status to hash
*/
void tcp_v4_hash_add_listen_pcb(struct netif *where, struct tcp_pcb_listen *who, unsigned short local_port)
{
	struct inet_hashinfo *hashinfo = where->tcp_hash;	
	struct inet_listen_hashbucket *ilb;
	unsigned int hash = inet_lhashfn(where, local_port);

	/* A bit like __inet_hash */
	ilb = &hashinfo->listening_hash[hash];
	hlist_nulls_add_head(&who->pcb_nulls_node, &ilb->head);
}

/**
	@brief Remove Active/TW pcb from hash list 
*/
void tcp_v4_hash_remove(struct tcp_pcb *who)
{	
	/* Sanity check */
	if (who->hash_list_in != HASH_LIST_ACTIVE &&
		who->hash_list_in != HASH_LIST_TW)
		return;
	hlist_nulls_del(&who->pcb_nulls_node);
	who->hash_list_in = HASH_LIST_UNKNOWN;
}

/**
	@brief Remove pcb from listen status
*/
void tcp_v4_hash_remove_listen(struct tcp_pcb_listen *who)
{
	/* Sanity check */
	if (who->hash_list_in != HASH_LIST_LISTEN)
		return;
	hlist_nulls_del(&who->pcb_nulls_node);
	who->hash_list_in = HASH_LIST_UNKNOWN;
}
