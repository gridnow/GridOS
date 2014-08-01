#include <loongson.h>
#include <compiler.h>
#include <types.h>
#include <hal_print.h>
#include <hal_debug.h>
#include <hal_bitops.h>

#include <private/interrupt.h>
#include <private/irq.h>

#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/i8259.h>
#include <ASM/MIPSREGs.h>

#include "htregs.h"

void irq_dispatch_loongson_3(unsigned int pending)
{
	if (pending & CAUSEF_IP7) 
	{
		TRACE_UNIMPLEMENTED("IP7");
		do_IRQ(MIPS_CPU_IRQ_BASE + 7);		
	}
	else if (pending & CAUSEF_IP6)
	{
		/* IPI */
		loongson3_ipi_interrupt(); 
	}
	else if (pending & CAUSEF_IP3) 
	{
		int i;
		int irq = HT_irq_vector_reg0;
		HT_irq_vector_reg0 = irq;
		
		/* ht */
		while((i = ffs(irq)))
		{
			do_IRQ(i - 1);
			irq &= ~(1 << (i - 1));
		}
	}
	else
	{
		/* Spurious */
		TRACE_UNIMPLEMENTED("Spurious");
	}
}

static struct irqaction cascade_irqaction = {
	.handler = no_action,
	.name = "cascade",
	.flags = IRQF_SHARED,
};

void __init init_irq_loongson3(void)
{
	unsigned int mask;
	
	/* init all controller
	 *   0-15         ------> i8259 interrupt
	 *   16-23        ------> mips cpu interrupt
	 */

	/* Sets the first-level interrupt dispatcher. */
	mips_cpu_irq_init();
	ht_irq_init();	

	setup_irq(MIPS_CPU_IRQ_BASE + 3, &cascade_irqaction);
	set_c0_status(STATUSF_IP6);
}
