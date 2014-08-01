#include <ddk/debug.h>

#include <types.h>
#include <compiler.h>
#include <irq.h>

#include <asm/irq.h>
#include <asm/mipsregs.h>
#include <asm/mipsmtregs.h>
#include <asm/cpu-features.h>

static inline void unmask_mips_irq(struct irq_data *d)
{
	set_c0_status(0x100 << (d->irq - MIPS_CPU_IRQ_BASE));
	irq_enable_hazard();
}

static inline void mask_mips_irq(struct irq_data *d)
{
	clear_c0_status(0x100 << (d->irq - MIPS_CPU_IRQ_BASE));
	irq_disable_hazard();
}

static struct irq_chip mips_cpu_irq_controller = {
	.name		= "MIPS",
	.irq_ack		= mask_mips_irq,
	.irq_mask		= mask_mips_irq,
	.irq_mask_ack	= mask_mips_irq,
	.irq_unmask		= unmask_mips_irq,
	.irq_eoi		= unmask_mips_irq,
};

/*
 * Basically the same as above but taking care of all the MT stuff
 */

static unsigned int mips_mt_cpu_irq_startup(struct irq_data *d)
{
	unsigned int vpflags = dvpe();

	clear_c0_cause(0x100 << (d->irq - MIPS_CPU_IRQ_BASE));
	evpe(vpflags);
	unmask_mips_irq(d);
	return 0;
}

/*
 * While we ack the interrupt interrupts are disabled and thus we don't need
 * to deal with concurrency issues.  Same for mips_cpu_irq_end.
 */
static void mips_mt_cpu_irq_ack(struct irq_data *d)
{
	unsigned int vpflags = dvpe();
	clear_c0_cause(0x100 << (d->irq - MIPS_CPU_IRQ_BASE));
	evpe(vpflags);
	mask_mips_irq(d);
}

static struct irq_chip mips_mt_cpu_irq_controller = {
	.name		= "MIPS",
	.irq_startup	= mips_mt_cpu_irq_startup,
	.irq_ack	= mips_mt_cpu_irq_ack,
	.irq_mask	= mask_mips_irq,
	.irq_mask_ack	= mips_mt_cpu_irq_ack,
	.irq_unmask	= unmask_mips_irq,
	.irq_eoi	= unmask_mips_irq,
};
	
void __init mips_cpu_irq_init(void)
{
	int irq_base = MIPS_CPU_IRQ_BASE;
	int i;

	printk("mips_cpu_irq_init MIPS_CPU_IRQ_BASE = %d, I8259A_IRQ_BASE = %d\n", MIPS_CPU_IRQ_BASE, I8259A_IRQ_BASE);

	/* Mask interrupts. */
	clear_c0_status(ST0_IM);
	clear_c0_cause(CAUSEF_IP);

	/* Software interrupts are used for MT/CMT IPI */
	for (i = irq_base; i < irq_base + 2; i++)
		irq_set_chip_and_handler(i, cpu_has_mipsmt ?
					 &mips_mt_cpu_irq_controller :
					 &mips_cpu_irq_controller,
					 handle_percpu_irq);

	for (i = irq_base + 2; i < irq_base + 8; i++)
		irq_set_chip_and_handler(i, &mips_cpu_irq_controller,
					 handle_percpu_irq);
}
