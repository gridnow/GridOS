#ifndef ARM_PAGE_H
#define ARM_PAGE_H

#include <asm/proc-fns.h>

#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level.h"
#else
#include "pgtable-2level.h"
#endif
#include "pgtable-hwdef.h"
#include <asm/tlbflush.h>

#ifndef __ASSEMBLER__
#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level-types.h"
#else
#include "pgtable-2level-types.h"
#endif

#include <asm/domain.h>
#include <asm/cacheflush.h>
#include <asm/mmu.h>
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
	@brief Mach related protect mode
*/
extern pgprot_t		pgprot_user;
extern pgprot_t		pgprot_kernel;

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
	Error code of page fault
 */
#define PAGE_FAULT_P				(1<<0)
#define PAGE_FAULT_W				(1<<1)

/**
	For page table
*/
#define _PAGE_USER_TABLE	(PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_USER))
#define _PAGE_KERNEL_TABLE	(PMD_TYPE_TABLE | PMD_BIT4 | PMD_DOMAIN(DOMAIN_KERNEL))

/*
	For page walk
*/
#include "walk-2level.h"

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
	const struct mem_type *type;
	unsigned long arch_flags = 0;
	
	if (!prot) 
		goto end;
	
	if (prot & KM_MAP_DEVICE)
	{
		type = get_mem_type(MT_DEVICE);
		arch_flags = type->prot_pte;
	}
	else if (prot & KM_MAP_ARCH_SPECIAL)
	{		
		type = get_mem_type(prot & 0xffff);
		arch_flags = type->prot_pte;
	}
	
	/*Or the normal*/
	else
	{
#if 0
		arch_flags = pgprot_kernel;
		//TODO: To support user level protection
		if (prot & KM_PROT_READ)		
			arch_flags |= PAGE_FLAG_VALID | L_PTE_RDONLY;
		if (prot & KM_PROT_WRITE)
			arch_flags |= PAGE_FLAG_VALID;
#endif
		printk("User specific flags not supported.\n");
	}
end:
	return arch_flags;
}

#define ARCH_HAS_KM_GET_VID
static inline unsigned long km_get_vid(unsigned long level, unsigned long va)
{
	if (sizeof(unsigned long) == 4)
	{
		switch (level)
		{
			case 2:
				va >>= 21;
				va &= (ARCH_KM_LV2_COUNT - 1);
				break;
				
			case 1:
				va >>= 12;
				va &= (ARCH_KM_LV1_COUNT - 1);
				break;
				
			default:
				va = -1;
		}
	}
	else
	{
		//TODO level3?
		va = -1;
	}
	
	return va;
}

/*
	For TLB and Cache
*/
#define arch_clean_pte_table(PTE_TABLE) do { \
	clean_dcache_area((void*)(unsigned long)(PTE_TABLE) + PTE_HWTABLE_OFF, PTE_HWTABLE_SIZE); \
} while(0)

#endif /*Assemmbler */

#endif
