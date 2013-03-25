#include <errno.h>

#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>
#include "internals.h"


void hal_irq_disable(unsigned int irq)
{
	disable_irq(irq);
}

void hal_irq_enable(unsigned int irq)
{
	enable_irq(irq);
}

int hal_irq_setup(unsigned int irq, struct irqaction *act)
{
	setup_irq(irq, act);
}

int hal_irq_request(unsigned int irq, irq_handler_t handler, unsigned long flags,
					const char *name, void *dev)
{
	return request_irq(irq, handler, flags, name, dev);
}

void hal_irq_free(unsigned int irq, void *dev_id)
{
	free_irq(irq, dev_id);
}

bool hal_irq_can_request(unsigned int irq, unsigned long irqflags)
{
	return can_request_irq(irq, irqflags);
}

void hal_irq_early_init()
{
	extern void early_irq_init();
	early_irq_init();
}