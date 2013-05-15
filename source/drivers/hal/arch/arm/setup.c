/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ARM 平台初始化
 *
 */
#include <compiler.h>

#include <asm/system_info.h>
#include <asm/cputype.h>

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

static void __init setup_processor(void)
{
	__cpu_architecture = __get_cpu_architecture();
	
#if 0
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
	
	snprintf(init_utsname()->machine, __NEW_UTS_LEN + 1, "%s%c",
			 list->arch_name, ENDIANNESS);
	snprintf(elf_platform, ELF_PLATFORM_SIZE, "%s%c",
			 list->elf_name, ENDIANNESS);
	elf_hwcap = list->elf_hwcap;
#ifndef CONFIG_ARM_THUMB
	elf_hwcap &= ~HWCAP_THUMB;
#endif
	
	feat_v6_fixup();
	
	cacheid_init();
	cpu_init();
#endif
}

void __init __arm_main0(char **cmdline)
{
	serial_puts("This is the first function.\n");
	paging_init();
	serial_puts("paging init ok.\n");
	
	arch_enable_mmu();
	while(1);
}

void __init __arm_main1()
{
	serial_puts("MMU opened.");
	while (1);

}