#include <ddk/debug.h>

#include <linkage.h>
#include <irq.h>
#include <irqdesc.h>

#include <asm/mipsregs.h>
#include <asm/irq.h>

/*
 * do_IRQ handles all normal device IRQ's (the special
 * SMP cross-CPU interrupts have their own specific
 * handlers).
 */
void do_IRQ(unsigned int irq)
{
	generic_handle_irq(irq);
	if (irq == 8 || irq == 0)
	{
		extern void kt_sched_tick();
		kt_sched_tick();
	}
}

asmlinkage void spurious_interrupt(void)
{
	printk("spurious_interrupt");
}

/*
 * 'what should we do if we get a hw irq event on an illegal vector'.
 * each architecture has to answer this themselves.
 */
void ack_bad_irq(unsigned int irq)
{
	smtc_im_ack_irq(irq);
	printk("unexpected IRQ # %d\n", irq);
}

atomic_t irq_err_count;

void arch_init_irq()
{
	/* Clear all interrupt */
	clear_c0_status(ST0_IM);
	clear_c0_cause(CAUSEF_IP);

	//plat_init_irq();
}