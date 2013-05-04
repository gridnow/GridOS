#ifndef ARM_PAGE_H
#define ARM_PAGE_H

#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level-types.h"
#else
#include "pgtable-2level-types.h"
#endif

#include <const.h>

#ifdef CONFIG_ARM_LPAE
#include "pgtable-3level.h"
#else
#include "pgtable-2level.h"
#endif
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

#endif