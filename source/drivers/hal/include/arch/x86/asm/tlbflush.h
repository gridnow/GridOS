/*
	TLB ops
 */
#ifndef _ARCH_TLB_H
#define _ARCH_TLB_H
#include "processor.h"

static inline void __native_flush_tlb_single(unsigned long addr)
{
	asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
static inline void __native_flush_tlb(void)
{
	native_write_cr3(native_read_cr3());
}
#define __flush_tlb_single(addr) __native_flush_tlb_single(addr)
#define __flush_tlb() __native_flush_tlb()

static inline void __flush_tlb_one(unsigned long addr)
{
	if (cpu_has_invlpg)
		__flush_tlb_single(addr);
	else
		__flush_tlb();
}
#endif
