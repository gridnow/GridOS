
#ifndef _ASM_PGTABLE_2LEVEL_TYPES_H
#define _ASM_PGTABLE_2LEVEL_TYPES_H

#ifndef __ASSEMBLER__
#include <types.h>
#endif

typedef u32 pteval_t;
typedef u32 pmdval_t;

typedef pteval_t pgprot_t;
#define ARCH_HAS_PTE_T
typedef unsigned long pte_t;

#define pgprot_val(x)   (x)
#define __pgprot(x)     (x)

/* Level 2's format */
typedef struct lv2_entry
{
	unsigned long odd, even;
} lv2_entry_t;

#define PTE_HWTABLE_PTRS	(ARCH_KM_LV1_COUNT)
#define PTE_HWTABLE_OFF		(PTE_HWTABLE_PTRS * sizeof(pte_t))
#define PTE_HWTABLE_SIZE	(PTE_HWTABLE_PTRS * sizeof(u32))

#endif	/* _ASM_PGTABLE_2LEVEL_TYPES_H */
