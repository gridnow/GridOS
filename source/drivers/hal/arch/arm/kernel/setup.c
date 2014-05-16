/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ARM 平台初始化
 *
 */
#include <compiler.h>
#include <debug.h>

#include <stddef.h>

#include <asm/system_info.h>
#include <asm/cputype.h>
#include <asm/procinfo.h>
#include <asm/cp15.h>
#include <asm/hwcap.h>
#include <asm/proc-fns.h>
#include <asm/ptrace.h>
#include <asm/cachetype.h>
#include <asm/cacheflush.h>
#include <asm/tlbflush.h>
#include <asm/mmu.h>
#include <asm/traps.h>

#include <mach/map.h>
#include <mach/arch.h>

#include <arch/arch.h>

unsigned int processor_id;
unsigned int __machine_arch_type __read_mostly;
unsigned int cacheid __read_mostly;
unsigned int __atags_pointer __initdata;
unsigned int system_rev;
unsigned int system_serial_low;
unsigned int system_serial_high;
unsigned int elf_hwcap __read_mostly;

#ifdef MULTI_CPU
struct processor processor __read_mostly;
#endif
#ifdef MULTI_TLB
struct cpu_tlb_fns cpu_tlb __read_mostly;
#endif
#ifdef MULTI_USER
struct cpu_user_fns cpu_user __read_mostly;
#endif
#ifdef MULTI_CACHE
struct cpu_cache_fns cpu_cache __read_mostly;
#endif
#ifdef CONFIG_OUTER_CACHE
struct outer_cache_fns outer_cache __read_mostly;
EXPORT_SYMBOL(outer_cache);
#endif

/*
 * Cached cpu_architecture() result for use by assembler code.
 * C code should use the cpu_architecture() function instead of accessing this
 * variable directly.
 */
int __cpu_architecture __read_mostly = CPU_ARCH_UNKNOWN;

struct stack {
	u32 irq[3];
	u32 abt[3];
	u32 und[3];
} ____cacheline_aligned;

static struct stack stacks[NR_CPUS];

static const char *cpu_name;
static const char *machine_name;
struct machine_desc *machine_desc __initdata;

static union { char c[4]; unsigned long l; } endian_test __initdata = { { 'l', '?', '?', 'b' } };
#define ENDIANNESS ((char)endian_test.l)


static const char *proc_arch[] = {
	"undefined/unknown",
	"3",
	"4",
	"4T",
	"5",
	"5T",
	"5TE",
	"5TEJ",
	"6TEJ",
	"7",
	"?(11)",
	"?(12)",
	"?(13)",
	"?(14)",
	"?(15)",
	"?(16)",
	"?(17)",
};

static int __get_cpu_architecture(void)
{
	int cpu_arch;
	
	if ((read_cpuid_id() & 0x0008f000) == 0) {
		cpu_arch = CPU_ARCH_UNKNOWN;
	} else if ((read_cpuid_id() & 0x0008f000) == 0x00007000) {
		cpu_arch = (read_cpuid_id() & (1 << 23)) ? CPU_ARCH_ARMv4T : CPU_ARCH_ARMv3;
	} else if ((read_cpuid_id() & 0x00080000) == 0x00000000) {
		cpu_arch = (read_cpuid_id() >> 16) & 7;
		if (cpu_arch)
			cpu_arch += CPU_ARCH_ARMv3;
	} else if ((read_cpuid_id() & 0x000f0000) == 0x000f0000) {
		unsigned int mmfr0;
		
		/* Revised CPUID format. Read the Memory Model Feature
		 * Register 0 and check for VMSAv7 or PMSAv7 */
		asm("mrc	p15, 0, %0, c0, c1, 4"
		    : "=r" (mmfr0));
		if ((mmfr0 & 0x0000000f) >= 0x00000003 ||
		    (mmfr0 & 0x000000f0) >= 0x00000030)
			cpu_arch = CPU_ARCH_ARMv7;
		else if ((mmfr0 & 0x0000000f) == 0x00000002 ||
				 (mmfr0 & 0x000000f0) == 0x00000020)
			cpu_arch = CPU_ARCH_ARMv6;
		else
			cpu_arch = CPU_ARCH_UNKNOWN;
	} else
		cpu_arch = CPU_ARCH_UNKNOWN;
	
	return cpu_arch;
}

int __pure cpu_architecture(void)
{	
	return __cpu_architecture;
}

static int cpu_has_aliasing_icache(unsigned int arch)
{
	int aliasing_icache;
	unsigned int id_reg, num_sets, line_size;
	
	/* PIPT caches never alias. */
	if (icache_is_pipt())
		return 0;
	
	/* arch specifies the register format */
	switch (arch) {
		case CPU_ARCH_ARMv7:
			asm("mcr	p15, 2, %0, c0, c0, 0 @ set CSSELR"
				: /* No output operands */
				: "r" (1));
			isb();
			asm("mrc	p15, 1, %0, c0, c0, 0 @ read CCSIDR"
				: "=r" (id_reg));
			line_size = 4 << ((id_reg & 0x7) + 2);
			num_sets = ((id_reg >> 13) & 0x7fff) + 1;
			aliasing_icache = (line_size * num_sets) > PAGE_SIZE;
			break;
		case CPU_ARCH_ARMv6:
			aliasing_icache = read_cpuid_cachetype() & (1 << 11);
			break;
		default:
			/* I-cache aliases will be handled by D-cache aliasing code */
			aliasing_icache = 0;
	}
	
	return aliasing_icache;
}

static void __init cacheid_init(void)
{
	unsigned int cachetype = read_cpuid_cachetype();
	unsigned int arch = cpu_architecture();
	
	if (arch >= CPU_ARCH_ARMv6) {
		if ((cachetype & (7 << 29)) == 4 << 29) {
			/* ARMv7 register format */
			arch = CPU_ARCH_ARMv7;
			cacheid = CACHEID_VIPT_NONALIASING;
			switch (cachetype & (3 << 14)) {
				case (1 << 14):
					cacheid |= CACHEID_ASID_TAGGED;
					break;
				case (3 << 14):
					cacheid |= CACHEID_PIPT;
					break;
			}
		} else {
			arch = CPU_ARCH_ARMv6;
			if (cachetype & (1 << 23))
				cacheid = CACHEID_VIPT_ALIASING;
			else
				cacheid = CACHEID_VIPT_NONALIASING;
		}
		if (cpu_has_aliasing_icache(arch))
			cacheid |= CACHEID_VIPT_I_ALIASING;
	} else {
		cacheid = CACHEID_VIVT;
	}
	
	printk("CPU: %s data cache, %s instruction cache\n",
		   cache_is_vivt() ? "VIVT" :
		   cache_is_vipt_aliasing() ? "VIPT aliasing" :
		   cache_is_vipt_nonaliasing() ? "PIPT / VIPT nonaliasing" : "unknown",
		   cache_is_vivt() ? "VIVT" :
		   icache_is_vivt_asid_tagged() ? "VIVT ASID tagged" :
		   icache_is_vipt_aliasing() ? "VIPT aliasing" :
		   icache_is_pipt() ? "PIPT" :
		   cache_is_vipt_nonaliasing() ? "VIPT nonaliasing" : "unknown");
}

extern struct proc_info_list *lookup_processor_type(unsigned int);

static void __init feat_v6_fixup(void)
{
	int id = read_cpuid_id();
	
	if ((id & 0xff0f0000) != 0x41070000)
		return;
	
	/*
	 * HWCAP_TLS is available only on 1136 r1p0 and later,
	 * see also kuser_get_tls_init.
	 */
	if ((((id >> 4) & 0xfff) == 0xb36) && (((id >> 20) & 3) == 0))
		elf_hwcap &= ~HWCAP_TLS;
}

/*
 * cpu_init - initialise one CPU.
 *
 * cpu_init sets up the per-CPU stacks.
 */
void cpu_init(void)
{
	unsigned int cpu = 0;
	struct stack *stk = &stacks[cpu];
	
	cpu_proc_init();
	
	/*
	 * Define the placement constraint for the inline asm directive below.
	 * In Thumb-2, msr with an immediate value is not allowed.
	 */
#ifdef CONFIG_THUMB2_KERNEL
#define PLC	"r"
#else
#define PLC	"I"
#endif
	
	/*
	 * setup stacks for re-entrant exception handlers
	 */
	__asm__ (
			 "msr	cpsr_c, %1\n\t"
			 "add	r14, %0, %2\n\t"
			 "mov	sp, r14\n\t"
			 "msr	cpsr_c, %3\n\t"
			 "add	r14, %0, %4\n\t"
			 "mov	sp, r14\n\t"
			 "msr	cpsr_c, %5\n\t"
			 "add	r14, %0, %6\n\t"
			 "mov	sp, r14\n\t"
			 "msr	cpsr_c, %7"
			 :
			 : "r" (stk),
			 PLC (PSR_F_BIT | PSR_I_BIT | IRQ_MODE),
			 "I" (offsetof(struct stack, irq[0])),
			 PLC (PSR_F_BIT | PSR_I_BIT | ABT_MODE),
			 "I" (offsetof(struct stack, abt[0])),
			 PLC (PSR_F_BIT | PSR_I_BIT | UND_MODE),
			 "I" (offsetof(struct stack, und[0])),
			 PLC (PSR_F_BIT | PSR_I_BIT | SVC_MODE)
			 : "r14");
}

static void __init setup_processor(void)
{	
	struct proc_info_list *list;
	
	/*
	 * locate processor in the list of supported processor
	 * types.  The linker builds this table for us from the
	 * entries in arch/arm/mm/proc-*.S
	 */
	list = lookup_processor_type(read_cpuid_id());
	if (!list) {
		printk("CPU configuration botched (ID %08x), unable "
		       "to continue.\n", read_cpuid_id());
		while (1);
	}
	
	cpu_name = list->cpu_name;
	__cpu_architecture = __get_cpu_architecture();
	
#ifdef MULTI_CPU
	processor = *list->proc;
#endif
#ifdef MULTI_TLB
	cpu_tlb = *list->tlb;
#endif
#ifdef MULTI_USER
	cpu_user = *list->user;
#endif
#ifdef MULTI_CACHE
	cpu_cache = *list->cache;
#endif
	
	printk("CPU: %s [%08x] revision %d (ARMv%s), cr=%08lx\n",
	       cpu_name, read_cpuid_id(), read_cpuid_id() & 15,
	       proc_arch[cpu_architecture()], cr_alignment);
	

	elf_hwcap = list->elf_hwcap;
#ifndef CONFIG_ARM_THUMB
	elf_hwcap &= ~HWCAP_THUMB;
#endif
	
	feat_v6_fixup();
	
	cacheid_init();
	cpu_init();
}

static void __init devicemaps_init(struct machine_desc *mdesc)
{
	struct map_desc map;
	unsigned long addr;
	void *vectors;
	
	vectors = km_page_alloc_kerneled(1);
	
	early_trap_init(vectors);
#if 0
	/*
	 * Create a mapping for the machine vectors at the high-vectors
	 * location (0xffff0000).  If we aren't using high-vectors, also
	 * create a mapping at the low-vectors virtual address.
	 */
	map.pfn = __phys_to_pfn(HAL_GET_BASIC_PHYADDRESS(vectors));
	map.virtual = 0xffff0000;
	map.length = PAGE_SIZE;
	map.type = MT_HIGH_VECTORS;
	create_mapping(&map);
	
	if (!vectors_high()) {
		map.virtual = 0;
		map.type = MT_LOW_VECTORS;
		create_mapping(&map);
	}
#endif
	/*
	 * Ask the machine support to map in the statically mapped devices.
	 */
	if (mdesc->map_io)
		mdesc->map_io();
	
	/*
	 * Finally flush the caches and tlb to ensure that we're in a
	 * consistent state wrt the writebuffer.  This also ensures that
	 * any write-allocated cache lines in the vector page are written
	 * back.  After this point, we can start to touch devices again.
	 */
	local_flush_tlb_all();
	flush_cache_all();
}

void __init __noreturn __arm_main0(char **cmdline)
{
	early_paging_init();
	
	/* Will not reach here */
	while(1);
}

void __init __noreturn __arm_main1()
{	
	/* Make sure printk can be used*/
	mmu_map_debug_device();

	/* OK, let's go through the HAL phase */
	hal_main();
}

/***********************************************************************
								HAL 接口
***********************************************************************/
void hal_arch_init(int step)
{
	switch (step)
	{
		case HAL_ARCH_INIT_PHASE_EARLY:
		{
			printk("In HAL_ARCH_INIT_PHASE_EARLY phase of ARM...\n");
				
			setup_processor();
			/* Init the page-related low level information */
			paging_init();
		
			break;
		}
		case HAL_ARCH_INIT_PHASE_MIDDLE:
		{
			/*
				Init machine after kernel is ready,
				Default to s3c6410, TODO: should get by machine id.
			 */
			extern struct machine_desc *get_s3c6410_machine_desc();
			extern void s3cfb_init_lcd();
			struct machine_desc *machine;
			
			printk("In HAL_ARCH_INIT_PHASE_MIDDLE phase of ARM...\n");
			machine = get_s3c6410_machine_desc();
			devicemaps_init(machine);

			/* 可以启动lcd, devicemaps_init已经初始化(应该放在machine的 map_io )*/
			s3cfb_init_lcd();
			break;
		}
		case HAL_ARCH_INIT_PHASE_LATE:
			break;
	}
}

const xstring hal_arch_get_name()
{
	return "ARM";
}

void hal_arch_setup_percpu(int cpu, unsigned long base)
{
	/* We do not have special register to set cpu base, just put to stack top */	
	arch_stack_thread_info()->cpu = (void*)base;
}
