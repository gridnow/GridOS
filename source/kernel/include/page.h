#ifndef PAGE_H
#define PAGE_H

#include <kernel/kernel.h>

struct km;
bool km_page_map_range(struct km *mem_dst, unsigned long start_va, unsigned long size, unsigned long physical_pfn, page_prot_t prot);
bool km_page_create(struct km *mem_dst, unsigned long address, page_prot_t prot);

/**
	@brief π≤œÌ“≥
 */
int km_page_share(struct km *dst, unsigned long dst_addr, struct km *src, unsigned long src_addr);

/* Default physical address in entry is at high part and is the high significant bits */
#ifndef ARCH_KM_ENTRY_PHYSICAL_MASK
#define ARCH_KM_ENTRY_PHYSICAL_MASK (~(PAGE_SIZE - 1))
#endif

#define KE_PAGE_INVALID_PTE(pte) (!pte || !(pte & PAGE_FLAG_VALID))
#define KE_PAGE_PHY_FROM_PTE(pte) ((pte) & ARCH_KM_ENTRY_PHYSICAL_MASK)

/* Result only used for sharing page ops */
#define KM_PAGE_SHARE_RESULT_OK				0
#define KM_PAGE_SHARE_RESULT_ERR			1
#define KM_PAGE_SHARE_RESULT_SRC_INVALID	-2

#endif
