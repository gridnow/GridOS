#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>
#include "internals.h"

/*
 * We wait here for a poller to finish.
 *
 * If the poll runs on this CPU, then we yell loudly and return
 * false. That will leave the interrupt line disabled in the worst
 * case, but it should never happen.
 *
 * We wait until the poller is done and then recheck disabled and
 * action (about to be disabled). Only if it's still active, we return
 * true and let the handler run.
 */
bool irq_wait_for_poll(struct irq_desc *desc)
{
#ifdef CONFIG_SMP
	do {
		raw_spin_unlock(&desc->lock);
		while (irqd_irq_inprogress(&desc->irq_data))
			cpu_relax();
		raw_spin_lock(&desc->lock);
	} while (irqd_irq_inprogress(&desc->irq_data));
	/* Might have been disabled in meantime */
	return !irqd_irq_disabled(&desc->irq_data) && desc->action;
#else
	return false;
#endif
}
