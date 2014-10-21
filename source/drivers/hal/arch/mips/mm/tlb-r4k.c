#include <types.h>
#include <smp.h>
#include <ddk/debug.h>

#include <asm/mipsregs.h>
#include <asm/system.h>

/*
 * Make sure all entries differ.  If they're not different
 * MIPS32 will take revenge ...
 */
#define UNIQUE_ENTRYHI(idx) (CKSEG0 + ((idx) << (PAGE_SHIFT + 1)))

/* Atomicity and interruptability */
#ifdef CONFIG_MIPS_MT_SMTC

#include <asm/smtc.h>
#include <asm/mipsmtregs.h>

#define ENTER_CRITICAL(flags) \
	{ \
	unsigned int mvpflags; \
	local_irq_save(flags);\
	mvpflags = dvpe()
#define EXIT_CRITICAL(flags) \
	evpe(mvpflags); \
	local_irq_restore(flags); \
	}
#else

#define ENTER_CRITICAL(flags) local_irq_save(flags)
#define EXIT_CRITICAL(flags) local_irq_restore(flags)

#endif /* CONFIG_MIPS_MT_SMTC */

#if defined(CONFIG_CPU_LOONGSON)
/*
 * LOONGSON2 has a 4 entry itlb which is a subset of dtlb,
 * unfortrunately, itlb is not totally transparent to software.
 */
#define FLUSH_ITLB write_c0_diag(4);

//#define FLUSH_ITLB_VM(vma) { if ((vma)->vm_flags & VM_EXEC)  write_c0_diag(4); }

#else

#define FLUSH_ITLB
//#define FLUSH_ITLB_VM(vma)

#endif

void local_flush_tlb_all(void)
{
	unsigned long flags;
	unsigned long old_ctx;
	int entry;

	ENTER_CRITICAL(flags);
	/* Save old context and create impossible VPN2 value */
	old_ctx = read_c0_entryhi();
	write_c0_entrylo0(0);
	write_c0_entrylo1(0);

	entry = read_c0_wired();

	/* Blast 'em all away. */
	while (entry < current_cpu_data.tlbsize) {
		/* Make sure all entries differ. */
		write_c0_entryhi(UNIQUE_ENTRYHI(entry));
		write_c0_index(entry);
		mtc0_tlbw_hazard();
		tlb_write_indexed();
		entry++;
	}
	tlbw_use_hazard();
	write_c0_entryhi(old_ctx);
	FLUSH_ITLB;
	EXIT_CRITICAL(flags);
}

/**
	@brief Flush all entries with this ASID
*/
void local_flush_asid(unsigned long asid)
{
	local_flush_tlb_all();
	//TODO :use the asid to flush
}

/**
	@brief Flush memory range

	If the memory range is too big, we flush all entries with this ASID
*/
void local_flush_tlb_range(unsigned int asid, unsigned long start, unsigned long end)
{
	unsigned long size, flags;

	ENTER_CRITICAL(flags);
	size = (end - start + (PAGE_SIZE - 1)) >> PAGE_SHIFT;
	size = (size + 1) >> 1;

	if (size <= current_cpu_data.tlbsize / 2)
	{
		int oldpid = read_c0_entryhi();
		int newpid = asid;

		start &= (PAGE_MASK << 1);
		end += ((PAGE_SIZE << 1) - 1);
		end &= (PAGE_MASK << 1);
		while (start < end) {
			int idx;

			write_c0_entryhi(start | newpid);
			start += (PAGE_SIZE << 1);
			mtc0_tlbw_hazard();
			tlb_probe();
			tlb_probe_hazard();
			idx = read_c0_index();
			write_c0_entrylo0(0);
			write_c0_entrylo1(0);
			if (idx < 0)
				continue;
			/* Make sure all entries differ. */
			write_c0_entryhi(UNIQUE_ENTRYHI(idx));
			mtc0_tlbw_hazard();
			tlb_write_indexed();
		}
		tlbw_use_hazard();
		write_c0_entryhi(oldpid);
	} 
	else
		local_flush_asid(asid);

	FLUSH_ITLB;
	EXIT_CRITICAL(flags);
}

void __cpuinit tlb_init(void)
{
	/*
	 * You should never change this register:
	 *   - On R4600 1.7 the tlbp never hits for pages smaller than
	 *     the value in the c0_pagemask register.
	 *   - The entire mm handling assumes the c0_pagemask register to
	 *     be set to fixed-size pages.
	 */
	write_c0_pagemask(PM_DEFAULT_MASK);
	write_c0_wired(0);
	if (current_cpu_type() == CPU_R10000 ||
	    current_cpu_type() == CPU_R12000 ||
	    current_cpu_type() == CPU_R14000)
		write_c0_framemask(0);

	if (kernel_uses_smartmips_rixi) {
		/*
		 * Enable the no read, no exec bits, and enable large virtual
		 * address.
		 */
		u32 pg = PG_RIE | PG_XIE;
#ifdef CONFIG_64BIT
		pg |= PG_ELPA;
#endif
		write_c0_pagegrain(pg);
	}

	//temp_tlb_entry = current_cpu_data.tlbsize - 1;
	printk("TLB ÊýÁ¿%d.\n", current_cpu_data.tlbsize);
        /* From this point on the ARC firmware is dead.  */
	local_flush_tlb_all();
	
	/* Did I tell you that ARC SUCKS?  */

// 	if (ntlb) {
// 		if (ntlb > 1 && ntlb <= current_cpu_data.tlbsize) {
// 			int wired = current_cpu_data.tlbsize - ntlb;
// 			write_c0_wired(wired);
// 			write_c0_index(wired-1);
// 			printk("Restricting TLB to %d entries\n", ntlb);
// 		} else
// 			printk("Ignoring invalid argument ntlb=%d\n", ntlb);
// 	}

// 	build_tlb_refill_handler();
}
