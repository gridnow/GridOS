#ifndef ASM_THREAD_H
#define ASM_THREAD_H

#ifndef __ASSEMBLY__
#include <types.h>
#include <asm/cpu-info.h>
#include <asm/processor.h>

struct kmt_arch_thread {
	/* Saved main processor registers. */
	unsigned long reg16;
	unsigned long reg17, reg18, reg19, reg20, reg21, reg22, reg23;
	unsigned long reg29, reg30, reg31;

	/* Saved cp0 stuff. */
	unsigned long cp0_status, reg28/*The kernel thread have to save $28, or destory this register in PIC code(modules)*/;

	/* Saved fpu/fpu emulator stuff. */
	struct mips_fpu_struct fpu;
#ifdef CONFIG_MIPS_MT_FPAFF
	/* Emulated instruction count */
	unsigned long emulated_fp;
	/* Saved per-thread scheduler affinity mask */
	cpumask_t user_cpus_allowed;
#endif /* CONFIG_MIPS_MT_FPAFF */

	/* Saved state of the DSP ASE, if available. */
	struct mips_dsp_state dsp;

	/* Saved watch register state, if available. */
	union mips_watch_reg_state watch;

	/* Other stuff associated with the thread. */
	unsigned long cp0_badvaddr;	/* Last user fault */
	unsigned long cp0_baduaddr;	/* Last kernel fault accessing USEG */
	unsigned long error_code;
	unsigned long irix_trampoline;  /* Wheee... */
	unsigned long irix_oldctx;
#ifdef CONFIG_CPU_CAVIUM_OCTEON
    struct octeon_cop2_state cp2 __attribute__ ((__aligned__(128)));
    struct octeon_cvmseg_state cvmseg __attribute__ ((__aligned__(128)));
#endif
	struct mips_abi *abi;

	/* ring0 stack */
	unsigned long sp0;
};

struct kt_arch_thread {
	struct kmt_arch_thread ctx;
	unsigned long preempt_count;
};

/************************************************************************/
/* ARCH specified                                                       */
/************************************************************************/
extern unsigned long *cpu_base_array[NR_CPUS];
#define KMT_ARCH_THREAD_CP0_STACK_SIZE PAGE_SIZE

static __always_inline void * kt_arch_get_current()
{
	unsigned long * cpu_base;
	int id;

	/* TODO: MIPS 没办法，只有根据CPU ID来索引一个数组来得到CPU环境块的基地址 */
	id = 0;
	cpu_base = cpu_base_array[id];
	/* Entry 0 is CURRENT thread, see km_cpu */
	return (void*) cpu_base[0];
}

/* 用硬件加速的方法获取到CPU指针 */
static __always_inline void * kt_arch_get_cpu()
{
	unsigned long * cpu_base;
	int id;

	/* TODO: MIPS 没办法，只有根据CPU ID来索引一个数组来得到CPU环境块的基地址 */
	id = 0;
	cpu_base = cpu_base_array[id];
	/* Entry 1 is CURRENT cpu, see km_cpu */
	return (void*) cpu_base[1];
}

#define kt_arch_current() ((struct kt_arch_thread *)kt_arch_get_current())
#else

/************************************************************************/
/* 汇编要引用几个寄存器，为此设置偏移                                   */
/************************************************************************/
#if (_MIPS_SZLONG == 64)
#define LS 8
#endif
#if (_MIPS_SZLONG == 32)
#define LS 4
#endif

#define THREAD_REG16 (0 * LS)
#define THREAD_REG17 (1 * LS)
#define THREAD_REG18 (2 * LS)
#define THREAD_REG19 (3 * LS)
#define THREAD_REG20 (4 * LS)
#define THREAD_REG21 (5 * LS)
#define THREAD_REG22 (6 * LS)
#define THREAD_REG23 (7 * LS)

#define THREAD_REG29 (8 * LS)
#define THREAD_REG30 (9 * LS)
#define THREAD_REG31 (10 * LS)

#define THREAD_STATUS (11 * LS)
#define THREAD_REG28 (12 * LS)
#endif /* 汇编 */

#endif /* ASM_THREAD_H */
