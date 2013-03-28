/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   X86 32 C 入口
*/
#include <arch/arch.h>

#include <asm/asm.h>
#include <asm/serial.h>
#include <asm/processor.h>
#include <asm/processor_probe.h>
#include <asm/processor-flags.h>
#include <asm/system.h>
#include <asm/tsc.h>
#include <asm/apic.h>
#include <asm/smp.h>
#include <asm/irq_vectors.h>
#include <asm/desc.h>
#include <asm/irq.h>

#include <screen.h>


/************************************************************************/
/* INIT                                                                 */
/************************************************************************/
static unsigned long __init get_loaded_base()
{
	unsigned long base = 0;
#ifdef CONFIG_X86_32
	base = (unsigned long )&get_loaded_base;
	base &= 0xf0000000;
#endif
	return base;
}
/*
	只支持大页的情况,线性映射整个内存
*/
unsigned char initial_pagetable[PAGE_SIZE] __aligned(PAGE_SIZE) ={1/*must not in BSS, we will clean the bss after mapping*/};
#define FAK_ARCH_X86_INIT_PGTABLE	((unsigned long)initial_pagetable &0x00ffffff/**/ )
static int __init x86_create_initial_map()
{
	unsigned long phy= 0; 
	unsigned long *p = (unsigned long *)FAK_ARCH_X86_INIT_PGTABLE;				//The kernel topmost table
	int i;
	for (i = 0; i < 1024; i++) p[i] = 0;

	/*
		Kernel part in 4mb page;
	*/
	for (i = 0; i < 1024; i++)
	{	
		if (i == get_loaded_base() / 0x400000) phy = 0;							//如果到了内核的地址，还是从0开始映射,因为内核装的物理地址实际是在开头；
		p[i] = phy | 0x83;														//0x183 is the global
		phy += 0x400000;
	}
	write_cr3((unsigned long)p);

	phy = read_cr4();
	phy |= X86_CR4_PSE | X86_CR4_PGE;											//允许4MB页
	write_cr4(phy);

	phy = read_cr0();
	phy |= X86_CR0_PG;															//打开页表;
	phy &= ~(X86_CR0_CD | X86_CR0_NW);											//允许缓存；
	write_cr0(phy);

	return 0;
}

/*必须是INLINE的，保证fak_jump_to_32bit_high直接回到调用者,否则本函数的地址是对了，重新返回时就又回到了调用者的地址了*/
static void inline __init tramph_to_high_kernel_address() 
{
	unsigned long new_pos;
	unsigned int new_cs = __KERNEL_CS;
	extern void x86_jump_to_32bit_high(unsigned long, unsigned long, unsigned long);

	/*Use the new gdt entry as the kernel cs/ss*/	
	new_pos = get_loaded_base();
	x86_jump_to_32bit_high(new_cs, new_pos, __KERNEL_DS);
}

/**
	@brief init the arch early

	Called at head.S and the stack will be changed to kernel before tramp_to_high
*/
void __init arch_early_init()
{
	extern char __bss_stop[], __bss_start[];
	x86_create_initial_map();

	memset(__bss_start, 0, __bss_stop - __bss_start);
	load_gdt_idt(0);
}

void __init tramp_to_high()
{
	tramph_to_high_kernel_address();
	
	hal_main();
}


/************************************************************************/
/* CPU                                                                  */
/************************************************************************/
/**
	@brief prepare goto the smp mode 
*/
static void x86_prepare_smp()
{
	/* Find and Analyze the table from BIOS AND ACPI*/
	mptable_find_smp_config();
	mptable_get_smp_config();

	/* This is called in init_IRQ */
	init_bsp_APIC();

	/* Connect apic */
	connect_bsp_APIC();	
	setup_local_APIC();
	bsp_end_local_APIC_setup();
	printk("\n%s->%s->%d.",__FILE__,__FUNCTION__,__LINE__);
}

/************************************************************************/
/* EXPORT                                                              */
/************************************************************************/

void hal_arch_cpu_up(int cpu)
{
	smp_ops.cpu_up( cpu );
}

void hal_arch_init(int step)
{
	switch (step)
	{
	case HAL_ARCH_INIT_PHASE_EARLY:
		x86_serial_init();
		early_cpu_init();
		hal_irq_early_init();
		arch_init_irq();
		arch_trap_init();
		printk("\n%s->%s->%d.",__FILE__,__FUNCTION__,__LINE__);
		time_init();
		printk("\n%s->%s->%d.",__FILE__,__FUNCTION__,__LINE__);
		break;
	case HAL_ARCH_INIT_PHASE_MIDDLE:
		arch_init_irq();
		break;
	case HAL_ARCH_INIT_PHASE_LATE:
		/* start smp and run cpu_init on each cpu */
		cpu_init();

		//x86_prepare_smp();		
		break;
	}
}

const xstring arch_hal_get_name()
{
#if defined(__i386__)
	return "x86 32 bits";
#elif defined (__x86_64__)
	return "x86 64 bits";
#else
	return "x86 unknowing";
#endif
}

void hal_arch_video_init_screen(struct video_screen_info *main_screen)
{
	unsigned long fb;
	int w, h, bpp;
	memset(main_screen, 0, sizeof(*main_screen));

	
	//TODO: 通过引导程序填充该数据结构。
	main_screen->bpp 			= 0;
	main_screen->fb_physical 	= 0;
	main_screen->height 		= 0;
	main_screen->width 			= 0;	
}


void arch_get_fw_arg(unsigned long *argc, unsigned long *argv)
{
	if (argc) *argc = NULL;
	if (argv) *argv = NULL;
}

int arch_get_fw_arg_pointer_size()
{
	return 0;
}

/************************************************************************/
/* PRIVATE to x86                                                       */
/************************************************************************/
void * hal_x86_get_init_pgtable()
{
	return initial_pagetable;
}
