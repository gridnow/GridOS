#include <ddk/debug.h>

#include <types.h>
#include <compiler.h>
#include <smp.h>

#include <asm/addrspace.h>
#include <asm/mipsregs.h>
#include <asm/cacheflush.h>
#include <asm/mipsmtregs.h>
#include <asm/stacktrace.h>
#include <asm/ptrace.h>

static int __cpuinitdata noulri;
unsigned long ebase;
#define VECTORSPACING 0x100	/* for EI/VI mode */
/*
 * The architecture-independent dump_stack generator
 */
void dump_stack(void)
{
	struct pt_regs regs;

	prepare_frametrace(&regs);
	//show_backtrace(current, &regs);
}
/*
 * Install uncached CPU exception handler.
 * This is suitable only for the cache error exception which is the only
 * exception handler that is being run uncached.
 */
void set_uncached_handler(unsigned long offset, void *addr, unsigned long size)
{
	unsigned long uncached_ebase = CKSEG1ADDR(ebase);	
	if (!addr)
	{
		hal_printf_error("UNCACHE处理函数为NULL.");
		return;		
	}
	memcpy((void *)(uncached_ebase + offset), addr, size);
}

void set_handler(unsigned long offset, void *addr, unsigned long size)
{
	memcpy((void *)(ebase + offset), addr, size);
	local_flush_icache_range(ebase + offset, ebase + offset + size);
}

/* 应该在每个处理器上跑一边 */
extern void cpu_cache_init(void);
extern void tlb_init(void);

void __cpuinit per_cpu_trap_init(void)
{
	unsigned int __maybe_unused cpu = smp_processor_id();
	unsigned int status_set = ST0_CU0;
	unsigned int hwrena = cpu_hwrena_impl_bits;
#ifdef CONFIG_MIPS_MT_SMTC
	int secondaryTC = 0;
	int bootTC = (cpu == 0);

	/*
	 * Only do per_cpu_trap_init() for first TC of Each VPE.
	 * Note that this hack assumes that the SMTC init code
	 * assigns TCs consecutively and in ascending order.
	 */

	if (((read_c0_tcbind() & TCBIND_CURTC) != 0) &&
	    ((read_c0_tcbind() & TCBIND_CURVPE) == cpu_data[cpu - 1].vpe_id))
		secondaryTC = 1;
#endif /* CONFIG_MIPS_MT_SMTC */

	/*
	 * Disable coprocessors and select 32-bit or 64-bit addressing
	 * and the 16/32 or 32/32 FPR register model.  Reset the BEV
	 * flag that some firmware may have left set and the TS bit (for
	 * IP27).  Set XX for ISA IV code to work.
	 */
#ifdef CONFIG_64BIT
	status_set |= ST0_FR|ST0_KX|ST0_SX|ST0_UX;
#endif
	if (current_cpu_data.isa_level == MIPS_CPU_ISA_IV)
		status_set |= ST0_XX;
	if (cpu_has_dsp)
		status_set |= ST0_MX;

	change_c0_status(ST0_CU|ST0_MX|ST0_RE|ST0_FR|ST0_BEV|ST0_TS|ST0_KX|ST0_SX|ST0_UX,
			 status_set);

	if (cpu_has_mips_r2)
		hwrena |= 0x0000000f;

	if (!noulri && cpu_has_userlocal)
		hwrena |= (1 << 29);

	if (hwrena)
		write_c0_hwrena(hwrena);

#ifdef CONFIG_MIPS_MT_SMTC
	if (!secondaryTC) {
#endif /* CONFIG_MIPS_MT_SMTC */

	if (cpu_has_veic || cpu_has_vint) {
		unsigned long sr = set_c0_status(ST0_BEV);
		write_c0_ebase(ebase);
		write_c0_status(sr);
		/* Setting vector spacing enables EI/VI mode  */
		change_c0_intctl(0x3e0, VECTORSPACING);
	}
	if (cpu_has_divec) {
		if (cpu_has_mipsmt) {
			unsigned int vpflags = dvpe();
			set_c0_cause(CAUSEF_IV);
			evpe(vpflags);
		} else
			set_c0_cause(CAUSEF_IV);
	}
#ifdef CONFIG_MIPS_MT_SMTC
	}
#endif /* CONFIG_MIPS_MT_SMTC */

	/* Init the TLB & Cache on this CPU */
	tlb_init();	
	cpu_cache_init();
	check_wait();
}

void arch_trap_init(void)
{
	/* from kernel */
	extern void except_vec0();
	extern void except_vec1();
	extern void except_vec3_generic();
	extern void except_cache();
	
	/* 计算EBASE */
	if (cpu_has_veic || cpu_has_vint) {
		unsigned long __maybe_unused size = 0x200 + VECTORSPACING*64;
// 		ebase = (unsigned long)
// 			__alloc_bootmem(size, 1 << fls(size), 0);
		hal_panic("需要分配EBASE");
	} else {
		ebase = CKSEG0;
		if (cpu_has_mips_r2)
			ebase += (read_c0_ebase() & 0x3ffff000);
	}
	/* 引导CPU应该跑 */
	per_cpu_trap_init();
	
	/* 安装钩子 */
    set_uncached_handler(0x100, &except_cache, 0x80);
    set_handler(0x180, &except_vec3_generic, 0x80);
    set_handler(0x80, &except_vec1, 0x80);
    set_handler(0x0, &except_vec0, 0x80);
}
