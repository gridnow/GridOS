#include <errno.h>

#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>
#include "internals.h"

struct irq_desc irq_desc[NR_IRQS] = {
	[0 ... NR_IRQS-1] = {
		.handle_irq	= handle_bad_irq,
		.depth		= 1,
		.lock		= __RAW_SPIN_LOCK_UNLOCKED(irq_desc->lock),
	}
};

static void __init init_irq_default_affinity(void)
{
}

static void desc_set_defaults(unsigned int irq, struct irq_desc *desc)
{
	int cpu;

	desc->irq_data.irq = irq;
	desc->irq_data.chip = &no_irq_chip;
	desc->irq_data.chip_data = NULL;
	desc->irq_data.handler_data = NULL;
	irq_settings_clr_and_set(desc, ~0, _IRQ_DEFAULT_INIT_FLAGS);
	irqd_set(&desc->irq_data, IRQD_IRQ_DISABLED);
	desc->handle_irq = handle_bad_irq;
	desc->depth = 1;
	desc->name = NULL;
}

struct irq_desc *irq_to_desc(unsigned int irq)
{
	return (irq < NR_IRQS) ? irq_desc + irq : NULL;
}

struct irq_desc *__irq_get_desc_lock(unsigned int irq, unsigned long *flags, bool bus, unsigned int check)
{
  struct irq_desc *desc = irq_to_desc(irq);

  if (desc) {
	  if (bus)
		  chip_bus_lock(desc);
	  raw_spin_lock_irqsave(&desc->lock, *flags);
  }
  return desc;
}

void __irq_put_desc_unlock(struct irq_desc *desc, unsigned long flags, bool bus)
{
  raw_spin_unlock_irqrestore(&desc->lock, flags);
  if (bus)
	  chip_bus_sync_unlock(desc);
}

/* Invoke the handler for a particular irq */
int generic_handle_irq(unsigned int irq)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (!desc)
		return -EINVAL;
	generic_handle_irq_desc(irq, desc);
	return 0;
}

int __init early_irq_init(void)
{
	int count, i;
	struct irq_desc *desc;
	
	desc = irq_desc;
	count = ARRAY_SIZE(irq_desc);

	for (i = 0; i < count; i++) {
		raw_spin_lock_init(&desc[i].lock);
		desc_set_defaults(i, &desc[i]);
	}
}

