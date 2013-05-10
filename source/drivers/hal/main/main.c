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
	ks_init();

	hal_time_init();
	
	hal_malloc_init();
	
	local_irq_enable();
	
	

	printk("Starting up modules...");
	ke_module_entry();
	printk("Hal startup ok.\n");
	kernel_test();
	while (1) dumy_idle_ops(0);
}

void __init __noreturn hal_main()
{
	/* ���ٺ���,˭Ϊ���֣������һ��*/
	hal_arch_init(HAL_ARCH_INIT_PHASE_EARLY);

	km_cluster_init();
	build_ram_list();

	hal_video_init_screen();
	

	/* IRQ,ƽ̨�ĳ�ʼ������ƽ̨���жϣ�����������Ϣ */	
	hal_irq_init();
	hal_arch_init(HAL_ARCH_INIT_PHASE_MIDDLE);

	/* DPC */
	hal_dpc_init();
	hal_timer_init();
	hal_hardware_init();
	hal_console_init();

	/* ƽ̨���ڳ�ʼ��,�����������������������������ж�*/
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

void hal_do_panic(char *why)
{
	printk("�ں��쳣:%s.\n", why);
}

void ke_panic(char *why)
{
	hal_do_panic(why);
}