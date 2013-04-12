/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Main
*/
#include <irqflags.h>
#include <arch/arch.h>

static void build_ram_list()
{
	unsigned long start = 0x1000000, size = 128*1024*1024, node = 0;
	int count = 0;
	
	//TODO: call the arch to add memory
	km_insert_ram(start, size, node);

 	kc_init();
	kp_init();
	kt_init();

	hal_time_init();
	
	hal_malloc_init();
	
	local_irq_enable();

	printk("Starting up modules...");
	ke_module_entry();
	printk("Hal startup ok.\n");
	
	while (1) dumy_idle_ops(0);
}

void __init __noreturn hal_main()
{
	/* 开辟鸿蒙,谁为情种？最初的一切*/
	hal_arch_init(HAL_ARCH_INIT_PHASE_EARLY);

	km_cluster_init();
	build_ram_list();

	hal_video_init_screen();
	

	/* IRQ,平台的初始化，如平台的中断，各种配置信息 */	
	hal_irq_init();
	hal_arch_init(HAL_ARCH_INIT_PHASE_MIDDLE);

	/* DPC */
	hal_dpc_init();
	hal_timer_init();
	hal_hardware_init();
	hal_console_init();

	/* 平台后期初始化,比如最终启动其他处理器，并开中断*/
	hal_arch_init(HAL_ARCH_INIT_PHASE_LATE);

	/* Manager */
	kma_setup();
	kmm_init();
	kmp_init();
	ko_init();
	kp_init();
	ks_init();	

	while(1)
	{
	}
}

void ke_panic(char *why)
{
	printk("内核异常:%s.\n", why);
}