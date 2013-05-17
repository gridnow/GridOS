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

struct mm_struct
{
	unsigned int asid;
};
#ifdef CONFIG_CPU_HAS_ASID
#define ASID_BITS	8
#define ASID_MASK	((~0ULL) << ASID_BITS)
#define ASID(mm)	((mm)->asid & ~ASID_MASK)
#else
#define ASID(mm)	(0)
#endif
struct vm_area_struct
{
	struct mm_struct *vm_mm;
};

#endif /*Assemmbler */
#endif