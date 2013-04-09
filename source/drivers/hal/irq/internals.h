#define istate core_internal_state__do_not_mess_with_it

/*
 * Bit masks for desc->state
 *
 * IRQS_AUTODETECT		- autodetection in progress
 * IRQS_SPURIOUS_DISABLED	- was disabled due to spurious interrupt
 *				  detection
 * IRQS_POLL_INPROGRESS		- polling in progress
 * IRQS_ONESHOT			- irq is not unmasked in primary handler
 * IRQS_REPLAY			- irq is replayed
 * IRQS_WAITING			- irq is waiting
 * IRQS_PENDING			- irq is pending and replayed later
 * IRQS_SUSPENDED		- irq is suspended
 */
enum {
	IRQS_AUTODETECT		= 0x00000001,
	IRQS_SPURIOUS_DISABLED	= 0x00000002,
	IRQS_POLL_INPROGRESS	= 0x00000008,
	IRQS_ONESHOT		= 0x00000020,
	IRQS_REPLAY		= 0x00000040,
	IRQS_WAITING		= 0x00000080,
	IRQS_PENDING		= 0x00000200,
	IRQS_SUSPENDED		= 0x00000800,
};

extern int __irq_set_trigger(struct irq_desc *desc, unsigned int irq,
		unsigned long flags);

static inline void chip_bus_lock(struct irq_desc *desc)
{
	if (unlikely(desc->irq_data.chip->irq_bus_lock))
		desc->irq_data.chip->irq_bus_lock(&desc->irq_data);
}

static inline void chip_bus_sync_unlock(struct irq_desc *desc)
{
	if (unlikely(desc->irq_data.chip->irq_bus_sync_unlock))
		desc->irq_data.chip->irq_bus_sync_unlock(&desc->irq_data);
}

#define _IRQ_DESC_CHECK		(1 << 0)
#define _IRQ_DESC_PERCPU	(1 << 1)

#define IRQ_GET_DESC_CHECK_GLOBAL	(_IRQ_DESC_CHECK)
#define IRQ_GET_DESC_CHECK_PERCPU	(_IRQ_DESC_CHECK | _IRQ_DESC_PERCPU)

struct irq_desc *__irq_get_desc_lock(unsigned int irq, unsigned long *flags, bool bus, unsigned int check);
void __irq_put_desc_unlock(struct irq_desc *desc, unsigned long flags, bool bus);

static inline struct irq_desc *irq_get_desc_buslock(unsigned int irq, unsigned long *flags, unsigned int check)
{
	return __irq_get_desc_lock(irq, flags, true, check);
}

static inline void irq_put_desc_busunlock(struct irq_desc *desc, unsigned long flags)
{
	__irq_put_desc_unlock(desc, flags, true);
}

static inline struct irq_desc *irq_get_desc_lock(unsigned int irq, unsigned long *flags, unsigned int check)
{
	return __irq_get_desc_lock(irq, flags, false, check);
}

static inline void irq_put_desc_unlock(struct irq_desc *desc, unsigned long flags)
{
	__irq_put_desc_unlock(desc, flags, false);
}

//chip.c
extern int irq_startup(struct irq_desc *desc, bool resend);
extern void irq_enable(struct irq_desc *desc);
extern void irq_disable(struct irq_desc *desc);
extern void mask_irq(struct irq_desc *desc);
extern void unmask_irq(struct irq_desc *desc);
extern void irq_shutdown(struct irq_desc *desc);


//manage.c
extern void __disable_irq(struct irq_desc *desc, unsigned int irq, bool susp);
extern void __enable_irq(struct irq_desc *desc, unsigned int irq, bool resume);

//resend.c
void check_irq_resend(struct irq_desc *desc, unsigned int irq);

//spurious.c
bool irq_wait_for_poll(struct irq_desc *desc);

//handle.c
irqreturn_t handle_irq_event(struct irq_desc *desc);
extern void handle_bad_irq(unsigned int irq, struct irq_desc *desc);

/* desc status */
#include "settings.h"

/*
 * Manipulation functions for irq_data.state
 */
static inline void irqd_set_move_pending(struct irq_data *d)
{
	d->state_use_accessors |= IRQD_SETAFFINITY_PENDING;
}

static inline void irqd_clr_move_pending(struct irq_data *d)
{
	d->state_use_accessors &= ~IRQD_SETAFFINITY_PENDING;
}

static inline void irqd_clear(struct irq_data *d, unsigned int mask)
{
	d->state_use_accessors &= ~mask;
}

static inline void irqd_set(struct irq_data *d, unsigned int mask)
{
	d->state_use_accessors |= mask;
}

static inline bool irqd_has_set(struct irq_data *d, unsigned int mask)
{
	return d->state_use_accessors & mask;
}

