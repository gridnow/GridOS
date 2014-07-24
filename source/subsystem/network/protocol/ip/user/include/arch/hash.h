/**
	Hash∆•≈‰£∫port°°£¶ IP
*/
#ifndef HASH_H
#define HASH_H

struct netif;
struct tcp_pcb;
struct tcp_pcb_listen;
struct inet_hashinfo;

/* Hash list the pcb located in, used for sanity check */
#define HASH_LIST_TW		3
#define HASH_LIST_ACTIVE	2
#define HASH_LIST_LISTEN	1
#define HASH_LIST_UNKNOWN	0

/**
	@brief Create the hash table for TCP v4
*/
void tcp_v4_hash_create(struct inet_hashinfo **hash);

/**
	@brief Add an active TCP's PCB to hash table
*/
void tcp_v4_hash_add_active_pcb(struct netif *where, struct tcp_pcb *who, u16_t rport, u16_t lport,
								u32 rip, u32 lip);
/**
	@brief Add a TW TCP's PCB to hash table
*/
void tcp_v4_hash_add_tw_pcb(struct netif *where, struct tcp_pcb *who, u16_t rport, u16_t lport,
								u32 rip, u32 lip);
/**
	@brief Add a PCB in listen status to hash
*/
void tcp_v4_hash_add_listen_pcb(struct netif *where, struct tcp_pcb_listen *who, unsigned short local_port);

/**
	@brief Find the pcb from Active hash list
*/
struct tcp_pcb *tcp_v4_hash_lookup_active_pcb(struct netif *where, u16_t rport, u16_t lport,	u32 rip, u32 lip);

/**
	@brief Find the pcb from TW hash list
*/
struct tcp_pcb *tcp_v4_hash_lookup_tw_pcb(struct netif *where, u16_t rport, u16_t lport,	u32 rip, u32 lip);

/**
	@brief Find the pcb from Listen hash list

	@param[in] where Where to find the PCB
	@param[in] local_port The port number to search
	@param[in] local_ip The ip address number to search
*/
struct tcp_pcb_listen *tcp_v4_hash_lookup_listen_pcb(struct netif *where, unsigned int short local_port, u32 local_ip);

/**
	@brief Remove Active/TW pcb from hash list

	@param[in] who The PCB to be removed from the hash list
*/
void tcp_v4_hash_remove(struct tcp_pcb *who);

/**
	@brief Remove pcb from listen status

	@param[in] who The PCB to be removed from the hash list
*/
void tcp_v4_hash_remove_listen(struct tcp_pcb_listen *who);

#endif