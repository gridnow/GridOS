#ifndef ARM_PAGE_H
#define ARM_PAGE_H

#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level.h"
#else
#include "pgtable-2level.h"
#endif

#ifndef __ASSEMBLER__
#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level-types.h"
#else
#include "pgtable-2level-types.h"
#endif

#include <const.h>

/*
 * Architecture ioremap implementation.
 */
#define MT_DEVICE		0
#define MT_DEVICE_NONSHARED	1
#define MT_DEVICE_CACHED	2
#define MT_DEVICE_WC		3
#define MT_UNCACHED		4

//TODO: should use BSP version
#define MT_CACHECLEAN		5
#define MT_MINICLEAN		6
#define MT_LOW_VECTORS		7
#define MT_HIGH_VECTORS		8
#define MT_MEMORY		9
#define MT_ROM			10
#define MT_MEMORY_NONCACHED	11
#define MT_MEMORY_DTCM		12
#define MT_MEMORY_ITCM		13
#define MT_MEMORY_SO		14
#define MT_MEMORY_DMA_READY	15
struct mem_type {
	pteval_t prot_pte;
	pmdval_t prot_l1;
	pmdval_t prot_sect;
	unsigned int domain;
};

struct cpu_user_fns {
	void (*cpu_clear_user_highpage)(void *page, unsigned long vaddr);
	void (*cpu_copy_user_highpage)(void *to_page, void *from_page,
								   unsigned long vaddr, void *vm_area);
};

/**
	@brief Define the entry flags in for Kernel
 */
#define PAGE_FLAG_VALID				L_PTE_PRESENT
#define PAGE_FLAG_FROM_OTHER		(1 << 11)
#define PAGE_FLAG_MASK				(0x3ff)

/**
	@brief Error code of page fault
 */
#define PAGE_FAULT_P				(1<<0)
#define PAGE_FAULT_W				(1<<1)

#endif /*Assemmbler */

#endif
