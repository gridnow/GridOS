
#ifndef _ASM_PGTABLE_2LEVEL_H
#define _ASM_PGTABLE_2LEVEL_H

/*
 * section address mask and size definitions.
 */
#define SECTION_SHIFT		20
#define SECTION_SIZE		(1UL << SECTION_SHIFT)
#define SECTION_MASK		(~(SECTION_SIZE-1))

/*
 * ARMv6 supersection address mask and size definitions.
 */
#define SUPERSECTION_SHIFT	24
#define SUPERSECTION_SIZE	(1UL << SUPERSECTION_SHIFT)
#define SUPERSECTION_MASK	(~(SUPERSECTION_SIZE-1))

/*
 * "Logical" Flags
 */
#define L_PTE_PRESENT		(_AT(pteval_t, 1) << 0)
#define L_PTE_VALID		(_AT(pteval_t, 1) << 0)		/* Valid */
#define L_PTE_YOUNG		(_AT(pteval_t, 1) << 1)
#define L_PTE_DIRTY		(_AT(pteval_t, 1) << 6)
#define L_PTE_RDONLY		(_AT(pteval_t, 1) << 7)
#define L_PTE_USER		(_AT(pteval_t, 1) << 8)
#define L_PTE_XN		(_AT(pteval_t, 1) << 9)
#define L_PTE_SHARED		(_AT(pteval_t, 1) << 10)	/* shared(v6), coherent(xsc3) */

/*
 * These are the memory types, defined to be compatible with
 * pre-ARMv6 CPUs cacheable and bufferable bits:   XXCB
 */
#define L_PTE_MT_UNCACHED	(_AT(pteval_t, 0x00) << 2)	/* 0000 */
#define L_PTE_MT_BUFFERABLE	(_AT(pteval_t, 0x01) << 2)	/* 0001 */
#define L_PTE_MT_WRITETHROUGH	(_AT(pteval_t, 0x02) << 2)	/* 0010 */
#define L_PTE_MT_WRITEBACK	(_AT(pteval_t, 0x03) << 2)	/* 0011 */
#define L_PTE_MT_MINICACHE	(_AT(pteval_t, 0x06) << 2)	/* 0110 (sa1100, xscale) */
#define L_PTE_MT_WRITEALLOC	(_AT(pteval_t, 0x07) << 2)	/* 0111 */
#define L_PTE_MT_DEV_SHARED	(_AT(pteval_t, 0x04) << 2)	/* 0100 */
#define L_PTE_MT_DEV_NONSHARED	(_AT(pteval_t, 0x0c) << 2)	/* 1100 */
#define L_PTE_MT_DEV_WC		(_AT(pteval_t, 0x09) << 2)	/* 1001 */
#define L_PTE_MT_DEV_CACHED	(_AT(pteval_t, 0x0b) << 2)	/* 1011 */
#define L_PTE_MT_MASK		(_AT(pteval_t, 0x0f) << 2)

#ifndef __ASSEMBLY__
#define set_pte_ext(ptep,pte,ext) cpu_set_pte_ext(ptep,pte,ext)
#endif /* __ASSEMBLY__ */

#endif /* _ASM_PGTABLE_2LEVEL_H */
