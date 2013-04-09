#include <hardirq.h>

#include <asm/irq_vectors.h>
#include <asm/desc.h>
#include <asm/irq.h>
#include <asm/hw_irq.h>
#include <asm/apic.h>

atomic_t irq_err_count;

asmregparm unsigned int do_IRQ(struct pt_regs *regs)
{
	/* high bit used in ret_from_ code  */
	unsigned vector = ~regs->orig_ax;
	unsigned irq;

	irq_enter();
	irq = vector_irq[vector];
 	if (!handle_irq(irq, regs)) {
 		ack_APIC_irq();
 	}
 	irq_exit();
 	return 1;
}