#include <linkage.h>
#include <compiler.h>

#include <section.h>
#include <arch/page.h>
#include <thread.h>
#include <memory.h>
#include <process.h>
#include <cpu.h>

#include <asm/system.h>
#include <asm/ptrace.h>
#include <asm/mipsregs.h>
/**
	@brief The initial page table for kernel
*/
void * init_pgtable;

/**
	@brief Just give an ASID number
*/
static void alloc_asid(struct kc_cpu * cpu, struct km * mm_ctx)
{	
	unsigned long r;

	/* 第一次创立内核进程时，我们得到了1，但是后面又经过了kmm_arch_ctx_init_kernel修正，因此这个地方返回1没有什么问题 */
	r = ++cpu->cpu.asid;

	/* ASID 0 is for kernel process */
	if ((r & CPU_PAGE_FALG_ASID_MASK) == 0)
		r = ++cpu->cpu.asid ;
	mm_ctx->asid = r;
}

/**
	@brief 换入一个线程的ASID

	@note 
		1,the caller must have Interrupt DISABLED!		
		2,all cp0 registers relative to the TLB will be destroyed!
		3,called on context switch
*/
static void flip_asid(struct kc_cpu * cpu, struct km * mem)
{
	struct km *old;
	unsigned long my_asid;
	extern void local_flush_asid(unsigned long asid);

	my_asid = mem->asid & CPU_PAGE_FALG_ASID_MASK;

	/* USE this asid directly if no conflict with other thread */
	old = cpu->cpu.asid_confict_table[my_asid]; 
	if(old == NULL)
	{
		cpu->cpu.asid_confict_table[my_asid] = mem;
		goto use_it;
	}
	else if(old == mem)
		goto use_it;

	/* 没有空闲的ASID，那么抢占冲突的ASID */
	printk("Flusing total asid\n");
	local_flush_asid(my_asid);
	cpu->cpu .asid_confict_table[my_asid] = mem;	

use_it:
	/* If is current asid, we do not need to change */
	write_c0_entryhi(my_asid);
	mtc0_tlbw_hazard();
}

/**
	@brief Get the arch protection bit
*/
unsigned long km_arch_get_flags(page_prot_t prot)
{
	unsigned long arch_flags = 0;
	if (!prot) return 0;

	/*For device?*/
	if (prot & KM_MAP_DEVICE)
	{
		arch_flags |= PAGE_FLAG_VALID | PAGE_FLAG_WRITE | PAGE_FLAG_UNCACHED;
	}

	/*Or the normal*/
	else
	{
		if (prot & KM_PROT_READ)			arch_flags |= PAGE_FLAG_VALID | PAGE_FLAG_CACHEABLE_NONCOHERENT;
		if (prot & KM_PROT_WRITE)			arch_flags |= PAGE_FLAG_WRITE | PAGE_FLAG_VALID | PAGE_FLAG_CACHEABLE_NONCOHERENT;
	}

	return arch_flags; 
} 

/**
	@brief Arch ctx init
*/
void km_arch_ctx_init(struct km * mm_ctx)
{
	struct kc_cpu * cpu = kc_get();
	alloc_asid(cpu, mm_ctx);
	kc_put();
}

void km_arch_ctx_deinit(struct km * mm_ctx)
{
	/* 被删除的进程可能在ASID Confliction table 中*/
	//TODO: 
}

void km_arch_init_for_kernel(struct km * mm_ctx)
{
	mm_ctx->asid = 0;
	write_c0_entryhi(mm_ctx->asid);
	mtc0_tlbw_hazard();
}

/**
	@brief Switch memory context

	@note	
		1, IRQ disabled
*/
void km_arch_ctx_switch(struct km * pre_ctx, struct km * next_ctx)
{
	/* Raw version is ok, we are in IRQ disabled mode */
	struct kc_cpu * cpu;

	/* But kernel ctx no need to flush */
	if (next_ctx == &kp_get_system()->mem_ctx)
		return;
	cpu = kc_get_raw();
	flip_asid(cpu, next_ctx);
}

/**
	@brief Flush the tlb range

	@note
		If the size is not page aligned, it will be rounded
*/
void arch_flush_tlb_range(struct km * mem, unsigned long start, unsigned long size)
{
	extern void local_flush_tlb_range(unsigned int asid, unsigned long start, unsigned long end);

	/* Hal disabled the IRQ while flushing */
	local_flush_tlb_range(mem->asid, start, start + size/*round by hal*/);
}

void km_arch_copy_kernel(struct km * mm_ctx, unsigned long address)
{
	/* Nothing */
}

