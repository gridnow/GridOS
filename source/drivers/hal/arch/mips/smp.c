#include <ddk/debug.h>

#include <types.h>
#include <linkage.h>
#include <compiler.h>

#include <asm/smp.h>

struct plat_smp_ops *mp_ops;
static int active_cpu_count = 1;

/**
	@brief First entry for secondary cpu
*/
asmlinkage __cpuinit void start_secondary(unsigned long a0, unsigned long a1)
{
	printk("处理器启动成功, a0 = %x, a1 = %d.\n", a0, a1);

	mp_ops->init_secondary();

	/* New CPU is up */
	smp_add_active_count();

	/* Tell the SMP module we have finished setup */
	mp_ops->finish_secondary();

	printk("but the secondary is stopped at start_secondary.\n");
	while(1);
}

/**
	@brief the platform SMP to this layer
*/
__cpuinit void register_smp_ops(struct plat_smp_ops *ops)
{
	mp_ops = ops;

	/* Call its setup interface */
	ops->smp_setup();
}

int smp_get_active_count()
{
	return active_cpu_count;
}

void smp_add_active_count()
{
	active_cpu_count++;
}

void smp_dec_active_count()
{
	active_cpu_count--;
}

/**
	@brief Boot the APs

	@return the CPU count that has been successfully booted
*/
__cpuinit int smp_boot_aps()
{
	int count = 0;	
	int total_count = 4;
	
	hal_printf("换新其他%d个处理器...", total_count);
	for (count = 1; count < total_count; count++)
	{
		mp_ops->boot_secondary(count, NULL);
	}
	hal_printf("OK.\n");

	return count;
}

/**
	@brief trigger the plat to setup the SMP
*/
void arch_init_smp()
{
	//plat_init_smp();
}

