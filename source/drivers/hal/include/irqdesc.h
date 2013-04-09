#ifndef HAL_IRQ_DESC_H
#define HAL_IRQ_DESC_H

#include <cache.h>
#include <lock.h>
#include <ddk/compatible.h>

struct irq_desc {
	struct irq_data		irq_data;
	irq_flow_handler_t	handle_irq;
	struct irqaction	*action;	/* IRQ action list */
	unsigned int		status_use_accessors;
	unsigned int		core_internal_state__do_not_mess_with_it;
	unsigned int		depth;		/* nested irq disables */
	unsigned int		wake_depth;	/* nested wake enables */
	unsigned int		irq_count;	/* For detecting broken IRQs */
	unsigned long		threads_oneshot;

	raw_spinlock_t		lock;
	const char		*name;
} ____cacheline_internodealigned_in_smp;

//irqdesc.c
extern struct irq_desc *irq_to_desc(unsigned int irq);

#ifdef CONFIG_GENERIC_HARDIRQS
static inline struct irq_chip *irq_desc_get_chip(struct irq_desc *desc)
{
	return desc->irq_data.chip;
}

static inline void generic_handle_irq_desc(unsigned int irq, struct irq_desc *desc)
{
	desc->handle_irq(irq, desc);
}

#endif

#endif
