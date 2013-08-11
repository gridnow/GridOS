#include <errno.h>
#include <ddk/irq.h>

#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>
#include "internals.h"

/* External driver subsystem may need it */
static int null_handler(void *ptregs, int irq)
{
	return 0;
}
int (*external_irq_handler)(void *pt_regs, int irq) = null_handler;

int hal_irq_request(unsigned int irq, irq_handler_t handler, unsigned long flags,
					const char *name, void *dev)
{
	return request_threaded_irq(irq, handler, NULL, flags, name, dev);
}

bool hal_irq_can_request(unsigned int irq, unsigned long irqflags)
{
	return can_request_irq(irq, irqflags);
}

void hal_setup_external_irq_handler(void *entry)
{
	external_irq_handler = entry;
}

void hal_irq_early_init()
{
	extern void early_irq_init();
	early_irq_init();
}
