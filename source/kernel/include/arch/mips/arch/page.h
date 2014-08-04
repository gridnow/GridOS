#ifndef ARCH_PAGE_H
#define ARCH_PAGE_H

#include <linkage.h>
#include <kernel/kernel.h>

struct km;
struct pt_regs;

/* CPU page flags, the PFN mask  */
#define PAGE_FLAG_GLOBAL					(1 << 0)				// For write, if this bit in e0e1 are all 1, G bit in PFN2 will bit 1
#define PAGE_FLAG_VALID						(1 << 1)
#define PAGE_FLAG_WRITE						(1 << 2)
#define PAGE_FLAG_CACHEABLE_NONCOHERENT		(3 << 3)
#define PAGE_FLAG_UNCACHED					(2 << 3)
#define PAGE_FLAG_UNCACHED_ACCELERATED		(7 << 3)

#define PAGE_FLAG_LO_HW_MASK				0x3f					// EntryLo hardware flag mask

#define PAGE_FLAG_FROM_OTHER				(1 << 6)				// The first bit of PFN in e0e1(see p103 of MIPS manual volume III), for software managing use,but 1kb will fail
#define PAGE_FLAG_SOFT_MASK					(PAGE_FLAG_FROM_OTHER)

/* Just for software table */
#define PAGE_FLAG_MASK						(0xfff)

#if (PAGE_SIZE < 4096)
#error "Page size must >= 4096 VPN2 and soft BIT is using"
#endif

/* Page fault code */
#define PAGE_FAULT_P				(1<<0)
#define PAGE_FAULT_W				(1<<1)

/**
	@brief The user space limit
*/
#define ARCH_USER_SPACE_START	0
#ifdef CONFIG_64BIT
#define ARCH_USER_SPACE_SIZE	0x200000000UL
#else
#define ARCH_USER_SPACE_SIZE	0x80000000UL
#endif

/* ASID mode TLB */
#define ARCH_HAS_ASID						1
#define CPU_PAGE_FALG_ASID_MASK				0xff

//setup.c
static inline void km_arch_trim() {}

//page.c
extern void * init_pgtable;
unsigned long km_arch_get_flags(page_prot_t prot);
/**
	@brief Add the entry to TLB, but X86 do not need it ,the hardware will handle it 
*/
void arch_flush_tlb_range(struct km * mem, unsigned long start, unsigned long size);

//fault.c
asmlinkage void do_page_fault(struct pt_regs *regs, unsigned long write, unsigned long address);
/************************************************************************/
/* TLB OPS                                                              */
/************************************************************************/

/**
	@brief 刷新当前地址空间中的地址在tlb中条目
 */
static inline void km_arch_flush_page(struct km *mem, unsigned long virtual_address)
{
	arch_flush_tlb_range(mem, virtual_address, PAGE_SIZE);
}

static inline void km_arch_flush_pages(struct km *mem, unsigned long start, unsigned long size)
{
	arch_flush_tlb_range(mem, start, size);
}

#include "walk.h"
#endif
