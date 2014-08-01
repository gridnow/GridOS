#include <types.h>
#include <hal_bitops.h>
#include <cmdline.h>
#include <linkage.h>
#include <hal_print.h>
#include <hal_debug.h>
#include <hal_string.h>

#include <loongson.h>
#include <asm/irq.h> 
#include <asm/mipsregs.h>

static void noop_irq_dispacth(unsigned int irq)
{
	printk("中断号:%d ：", irq);
	hal_panic("没有选定正确的平台，不知道如何分发中断。");
}
/*
 * the first level int-handler will jump here if it is a bonito irq
 */
void bonito_irqdispatch(void)
{
	u32 int_status;
	int i;

	/* workaround the IO dma problem: let cpu looping to allow DMA finish */
	int_status = LOONGSON_INTISR;
	while (int_status & (1 << 10)) {
		//udelay(1);
		delay();
		int_status = LOONGSON_INTISR;
	}

	/* Get pending sources, masked by current enables */
	int_status = LOONGSON_INTISR & LOONGSON_INTEN;

	if (int_status) {
		i = __ffs(int_status);
		do_IRQ(LOONGSON_IRQ_BASE + i);
	}
}
static void (*mach_irq_dispatch)(unsigned int) = noop_irq_dispacth;
asmlinkage void plat_irq_dispatch(void)
{
	unsigned int pending;

	pending = read_c0_cause() & read_c0_status() & ST0_IM;
	/* machine-specific plat_irq_dispatch */
	mach_irq_dispatch(pending);	
}

/* Top most irq init for the platform*/
void __init plat_init_irq(void)
{
	char *v;
	/*
	 * Clear all of the interrupts while we change the able around a bit.
	 * int-handler is not on bootstrap
	 */
	clear_c0_status(ST0_IM | ST0_BEV);

	/* no steer */
	LOONGSON_INTSTEER = 0;

	/*
	 * Mask out all interrupt by writing "1" to all bit position in
	 * the interrupt reset reg.
	 */
	LOONGSON_INTENCLR = ~0;

	/* machine specific irq init */
	if (hal_cmdline_get_string_value("PLAT", &v) == true)
	{
		/* 2f box 2f pc use */
		if (!strncmp(v, "2f", 2))
		{
			init_irq_2f();
			mach_irq_dispatch = irq_dispatch_2f;
			TODO("");
		}
		else if (!strncmp(v, "3a", 2))
		{
			init_irq_loongson3();
			mach_irq_dispatch = irq_dispatch_loongson_3;
		}
		else
		{
			TODO("平台参数给定不对，初始化不了硬件中断控制器");
		}
	}
}
