#ifndef ARM_PAGE_H
#define ARM_PAGE_H

#include <asm/proc-fns.h>

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
#include "pgtable-hwdef.h"
#include <asm/tlbflush.h>
#include <asm/domain.h>
#include <asm/cacheflush.h>
#include <const.h>

/**
	@brief User addressing space size
 */
#define TASK_SIZE CONFIG_HAL_KERNEL_BASE

/**
	@brief We have to support Architecture ioremap implementation.
 */
#include <asm/dma-mapping.h>

struct mem_type {
	pteval_t prot_pte;
	pmdval_t prot_l1;
	pmdval_t prot_sect;
	unsigned int domain;
};

/**
	@brief Define the entry flags in for Kernel
 */
#define PAGE_FLAG_VALID				L_PTE_VALID
#define PAGE_FLAG_FROM_OTHER		(1 << 3/*not used by logical*/)
#define PAGE_FLAG_USER				L_PTE_USER
#define PAGE_FLAG_MASK				(0xfff)
#define pte_val(pte)				(pte)
#define pte_present(pte)			(pte_val(pte) & L_PTE_PRESENT)
#define pte_present_user(pte)		(pte_present(pte) && (pte_val(pte) & L_PTE_USER))

/**
	@brief Error code of page fault
 */
#define PAGE_FAULT_P				(1<<0)
#define PAGE_FAULT_W				(1<<1)

/**
	@brief For page table
*/
#define _PAGE_USER_TABLE	(PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_USER))
#define _PAGE_KERNEL_TABLE	(PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_KERNEL))

/*
	For page walk, should be defined in x-level.h
*/
#define ARCH_HAS_PTE_T
typedef unsigned long pte_t;
#define ARCH_KM_LV1_COUNT	2048
#define ARCH_KM_LV2_COUNT	512
#define KM_WALK_MAX_LEVEL	2

#define PTE_HWTABLE_PTRS	(ARCH_KM_LV2_COUNT)
#define PTE_HWTABLE_OFF		(PTE_HWTABLE_PTRS * sizeof(pte_t))
#define PTE_HWTABLE_SIZE	(ARCH_KM_LV2_COUNT * sizeof(u32))

static inline void km_write_sub_table(unsigned long *table, int sub_id, unsigned long phyiscal)
{
	phyiscal = phyiscal + PTE_HWTABLE_OFF;
	
	/* 两个PTE 表相邻 */
	table[sub_id] = phyiscal | _PAGE_USER_TABLE;
	table[sub_id + 1] = (phyiscal + 1024) | _PAGE_USER_TABLE ;
	flush_pmd_entry(&table[sub_id]);
}

static inline void set_pte_at(pte_t *ptep, pte_t pteval, unsigned long addr)
{
	unsigned long ext = 0;
	
	/* If in user, we disable GLOBAL for this entry */
	if (addr < TASK_SIZE && pte_present_user(pteval)) {
		ext |= PTE_EXT_NG;
	}
	
	set_pte_ext(ptep, pteval, ext);
} 
#define write_pte(PTE, WHAT) set_pte_at(p, what, 0);

static inline unsigned long km_arch_get_flags(page_prot_t prot)
{
	unsigned long arch_flags = PAGE_FLAG_USER;
	if (!prot) return 0;
#if 0
	if (prot & KM_MAP_DEVICE)
	{
		arch_flags |= PAGE_FLAG_VALID|PAGE_FLAG_RW|PAGE_FLAG_PCD|PAGE_FLAG_PWT;
	}
	
	/*Or the normal*/
	else
	{
		if (prot & KM_PROT_READ)			arch_flags |= PAGE_FLAG_VALID;
		if (prot & KM_PROT_WRITE)			arch_flags |= PAGE_FLAG_RW | PAGE_FLAG_VALID;
	}
#endif
	return arch_flags;
}

/*
	For TLB and Cache
*/
#define arch_clean_pte_table(PTE_TABLE) do { \
	clean_dcache_area((unsigned long)(PTE_TABLE) + PTE_HWTABLE_OFF, PTE_HWTABLE_SIZE); \
} while(0)

#endif /*Assemmbler */

#endif
