#include <errno.h>

#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>

#include "internals.h"

#include <asm/abicall.h>

void handle_bad_irq(unsigned int irq, struct irq_desc *desc)
{
	//TODO
}

irqreturn_t no_action(int cpl, void * dev_id)
{
	return IRQ_NONE;
}

irqreturn_t handle_irq_event_percpu(struct irq_desc *desc, struct irqaction *action)
{
	irqreturn_t retval = IRQ_NONE;
	unsigned int random = 0, irq = desc->irq_data.irq;
	
	do {
		irqreturn_t res;

		res = ka_call_dynamic_module_entry(action->handler, irq, action->dev_id);
		
		//TODO: 不是所有中断处理函数都会开启中断，因此这里不一定每次都关中断
		local_irq_disable();

		switch (res) {
		case IRQ_WAKE_THREAD:
			//TODO to wakeup thread

			/* Fall through to add to randomness */
		case IRQ_HANDLED:
			random |= action->flags;
			break;

		default:
			break;
		}

		retval |= res;
		action = action->next;
	} while (action);

	//TODO 增加随机数的种子
	return retval;
}

irqreturn_t handle_irq_event(struct irq_desc *desc)
{
	struct irqaction *action = desc->action;
	irqreturn_t ret;

	desc->istate &= ~IRQS_PENDING;
	irqd_set(&desc->irq_data, IRQD_IRQ_INPROGRESS);
	raw_spin_unlock(&desc->lock);

	ret = handle_irq_event_percpu(desc, action);

	raw_spin_lock(&desc->lock);
	irqd_clear(&desc->irq_data, IRQD_IRQ_INPROGRESS);
	return ret;
}
