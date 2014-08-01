/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Main
*/
#include <irqflags.h>
#include <arch/arch.h>

/* If we have extern module */
__weak ke_module_entry()
{
	
}

static void *driver_pakcage = NULL;
static int driver_size = 0;
static int driver_package_id, last_package_id;
static void *start_driver_ctx(void *data, int size, char *name, int id)
{
	/* We got the driver package? And we got again, error */
	if (!strcmp(name, "linux.drv"))
	{
		driver_pakcage = data;
		driver_size = size;
		driver_package_id = id;
	}
	last_package_id = id;
}

static void build_ram_list()
{
	
#ifdef __arm__
	unsigned long node = 0;
	unsigned long reserved = 0x200000;
	unsigned long start = 0x50000000 + reserved;
	unsigned long size = 128*1024*1024 - reserved;
#else
	unsigned long start = 0x1000000, size = 128*1024*1024, node = 0;
#endif
	int count = 0;
	
	//TODO: call the arch to add memory
	km_insert_ram(start, size, node);
}

void __init __noreturn hal_main()
{
	/* 开辟鸿蒙,谁为情种？最初的一切*/
	km_cluster_init();
	build_ram_list();
	
	/* Go back to ARCH, we have inited the basic paging allocator */
	hal_arch_init(HAL_ARCH_INIT_PHASE_EARLY);
	
	/* KERNEL */
	kc_init();
	kp_init();
	ks_init();
	
	hal_malloc_init();
	hal_dpc_init();
	hal_time_init();

	hal_arch_init(HAL_ARCH_INIT_PHASE_MIDDLE);

	hal_console_init();
	printk("GridOS 启动中...\n");
	ke_module_entry();
	
	local_irq_enable();
	
	/* Driver pakcage loading, and it must be the last file */
	hal_boot_module_loop(start_driver_ctx);
	if (last_package_id == driver_package_id && driver_pakcage)
	{
		ke_startup_driver_process(driver_pakcage, driver_size);
	}
	else
	{
		if (driver_size)
			printk("Driver package is not the last one, BSS in it may overlay the useful file data after it...");
		else
			printk("No driver package was loaded...");
	}
	printk("Hal startup ok.\n");
	
	kernel_test();
	while (1) 
	{
		kt_schedule_driver();
		/* If have no process, sleep */
		if (!kt_schedule_running_count())
			dumy_idle_ops(0);
		else
			kt_schedule();
	}
}

void hal_do_panic(char *why)
{
	printk("内核异常:%s.\n内核Die。\n", why);
	while(1);
}

void ke_panic(char *why)
{
	hal_do_panic(why);
}

unsigned long hal_get_basic_kaddr(unsigned long phy)
{
	return HAL_GET_BASIC_KADDRESS(phy);
}

unsigned long hal_get_basic_phyaddr(void *vaddr)
{
	return HAL_GET_BASIC_PHYADDRESS(vaddr);
}

