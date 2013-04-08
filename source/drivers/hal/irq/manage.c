#include <errno.h>

#include <irq.h>
#include <irqdesc.h>
#include <hardirq.h>
#include <debug.h>
#include "internals.h"

#include <ddk/slab.h>

#include <asm/processor.h>

/*
 * Default primary interrupt handler for threaded interrupts. Is
 * assigned as primary handler when request_threaded_irq is called
 * with handler == NULL. Useful for oneshot interrupts.
 */
static irqreturn_t irq_default_primary_handler(int irq, void *dev_id)
{
	return IRQ_WAKE_THREAD;
}

/* wait for pending IRQ handlers (on other CPUs)  */
void synchronize_irq(unsigned int irq)
{
	//TODO
}

int __irq_set_trigger(struct irq_desc *desc, unsigned int irq,
		      unsigned long flags)
{
	struct irq_chip *chip = desc->irq_data.chip;
	int ret, unmask = 0;

	if (!chip || !chip->irq_set_type) {
		/*
		 * IRQF_TRIGGER_* but the PIC does not support multiple
		 * flow-types?
		 */
		printk("No set_type function for IRQ %d (%s)\n", irq,
				chip ? (chip->name ? : "unknown") : "unknown");
		return 0;
	}

	flags &= IRQ_TYPE_SENSE_MASK;

	if (chip->flags & IRQCHIP_SET_TYPE_MASKED) {
		if (!irqd_irq_masked(&desc->irq_data))
			mask_irq(desc);
		if (!irqd_irq_disabled(&desc->irq_data))
			unmask = 1;
	}

	/* caller masked out all except trigger mode flags */
	ret = chip->irq_set_type(&desc->irq_data, flags);

	switch (ret) {
	case IRQ_SET_MASK_OK:
		irqd_clear(&desc->irq_data, IRQD_TRIGGER_MASK);
		irqd_set(&desc->irq_data, flags);

	case IRQ_SET_MASK_OK_NOCOPY:
		flags = irqd_get_trigger_type(&desc->irq_data);
		irq_settings_set_trigger_mask(desc, flags);
		irqd_clear(&desc->irq_data, IRQD_LEVEL);
		irq_settings_clr_level(desc);
		if (flags & IRQ_TYPE_LEVEL_MASK) {
			irq_settings_set_level(desc);
			irqd_set(&desc->irq_data, IRQD_LEVEL);
		}

		ret = 0;
		break;
	default:
		printk("setting trigger mode %lu for irq %u failed (%pF)\n",
		       flags, irq, chip->irq_set_type);
	}
	if (unmask)
		unmask_irq(desc);
	return ret;
}

/*
 * Internal function to register an irqaction - typically used to
 * allocate special interrupts that are part of the architecture.
 */
static int
__setup_irq(unsigned int irq, struct irq_desc *desc, struct irqaction *new)
{
	struct irqaction *old, **old_ptr;
	const char *old_name = NULL;
	unsigned long flags, thread_mask = 0;
	int ret, nested, shared = 0;
	
	if (!desc)
		return -EINVAL;

	if (desc->irq_data.chip == &no_irq_chip)
		return -ENOSYS;
	/*
	 * The following block of code has to be executed atomically
	 */
	raw_spin_lock_irqsave(&desc->lock, flags);
	old_ptr = &desc->action;
	old = *old_ptr;
	if (old) {
		/*
		 * Can't share interrupts unless both agree to and are
		 * the same type (level, edge, polarity). So both flag
		 * fields must have IRQF_SHARED set and the bits which
		 * set the trigger type must match. Also all must
		 * agree on ONESHOT.
		 */

		if (!((old->flags & new->flags) & IRQF_SHARED) ||
		    ((old->flags ^ new->flags) & IRQF_TRIGGER_MASK) ||
		    ((old->flags ^ new->flags) & IRQF_ONESHOT)) {
			old_name = old->name;
			goto mismatch;
		}

		/* All handlers must agree on per-cpuness */
		if ((old->flags & IRQF_PERCPU) !=
		    (new->flags & IRQF_PERCPU))
			goto mismatch;

		/* add new interrupt at end of irq queue */
		do {
			old_ptr = &old->next;
			old = *old_ptr;
		} while (old);
		shared = 1;
	}

	if (!shared) 
	{
		//TODO:共享中断下使用等待队列的问题暂时保留
		//init_waitqueue_head(&desc->wait_for_threads);

		/* Setup the type (level, edge polarity) if configured: */		
		if (new->flags & IRQF_TRIGGER_MASK) {
			ret = __irq_set_trigger(desc, irq,
					new->flags & IRQF_TRIGGER_MASK);

			if (ret)
				goto out_mask;
		}

		desc->istate &= ~(IRQS_AUTODETECT | IRQS_SPURIOUS_DISABLED | \
				  IRQS_ONESHOT | IRQS_WAITING);
		irqd_clear(&desc->irq_data, IRQD_IRQ_INPROGRESS);

		if (new->flags & IRQF_PERCPU) {
			irqd_set(&desc->irq_data, IRQD_PER_CPU);
			irq_settings_set_per_cpu(desc);
		}

		if (new->flags & IRQF_ONESHOT)
			desc->istate |= IRQS_ONESHOT;

		if (irq_settings_can_autoenable(desc))			
			irq_startup(desc, 1);
		else
			/* Undo nested disables: */
			desc->depth = 1;
		/* Exclude IRQ from balancing if requested */
		if (new->flags & IRQF_NOBALANCING) {
			irq_settings_set_no_balancing(desc);
			irqd_set(&desc->irq_data, IRQD_NO_BALANCING);
		}
	} else if (new->flags & IRQF_TRIGGER_MASK) {
		unsigned int nmsk = new->flags & IRQF_TRIGGER_MASK;
		unsigned int omsk = irq_settings_get_trigger_mask(desc);

		if (nmsk != omsk)
			/* hope the handler works with current  trigger mode */
			printk("IRQ %d uses trigger mode %u; requested %u\n",
				   irq, nmsk, omsk);
	}
	new->irq = irq;
	*old_ptr = new;
	/*
	 * Check whether we disabled the irq via the spurious handler
	 * before. Reenable it and give it another chance.
	 */	 
	if (shared && (desc->istate & IRQS_SPURIOUS_DISABLED)) {
		desc->istate &= ~IRQS_SPURIOUS_DISABLED;
		__enable_irq(desc, irq, false);
	}

	raw_spin_unlock_irqrestore(&desc->lock, flags);
	return 0;

mismatch:
#ifdef CONFIG_DEBUG_SHIRQ
	if (!(new->flags & IRQF_PROBE_SHARED)) {
		printk(HAL_ERR "IRQ handler type mismatch for IRQ %d\n", irq);
		if (old_name)
			printk(HAL_ERR "current handler: %s\n", old_name);
		dump_stack();
	}
#endif
	ret = -EBUSY;

out_mask:
	raw_spin_unlock_irqrestore(&desc->lock, flags);

	return ret;
}

void __disable_irq(struct irq_desc *desc, unsigned int irq, bool suspend)
{
	if (suspend) {
		if (!desc->action || (desc->action->flags & IRQF_NO_SUSPEND))
			return;
		desc->istate |= IRQS_SUSPENDED;
	}

	if (!desc->depth++)
		irq_disable(desc);
}

static int __disable_irq_nosync(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_buslock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);

	if (!desc)
		return -EINVAL;
	__disable_irq(desc, irq, false);
	irq_put_desc_busunlock(desc, flags);
	return 0;
}

void disable_irq_nosync(unsigned int irq)
{
	__disable_irq_nosync(irq);
}

void __enable_irq(struct irq_desc *desc, unsigned int irq, bool resume)
{
	if (resume) {
		if (!(desc->istate & IRQS_SUSPENDED)) {
			if (!desc->action)
				return;
			if (!(desc->action->flags & IRQF_FORCE_RESUME))
				return;
			/* Pretend that it got disabled ! */
			desc->depth++;
		}
		desc->istate &= ~IRQS_SUSPENDED;
	}

	switch (desc->depth) {
	case 0:
err_out:
		WARN(1, HAL_WARNING "Unbalanced enable for IRQ %d\n", irq);
		break;
	case 1: {
		if (desc->istate & IRQS_SUSPENDED)
			goto err_out;
		/* Prevent probing on this irq: */
		irq_settings_set_noprobe(desc);
		irq_enable(desc);
		check_irq_resend(desc, irq);
		/* fall-through */
			}
	default:
		desc->depth--;
	}
}

/*
 * Internal function to unregister an irqaction - used to free
 * regular and special interrupts that are part of the architecture.
 */
static struct irqaction *__free_irq(unsigned int irq, void *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);
	struct irqaction *action, **action_ptr;
	unsigned long flags;

	WARN(in_interrupt(), "Trying to free IRQ %d from IRQ context!\n", irq);

	if (!desc)
		return NULL;

	raw_spin_lock_irqsave(&desc->lock, flags);

	/*
	 * There can be multiple actions per IRQ descriptor, find the right
	 * one based on the dev_id:
	 */
	action_ptr = &desc->action;
	for (;;) {
		action = *action_ptr;

		if (!action) {
			WARN(1, "Trying to free already-free IRQ %d\n", irq);
			raw_spin_unlock_irqrestore(&desc->lock, flags);

			return NULL;
		}

		if (action->dev_id == dev_id)
			break;
		action_ptr = &action->next;
	}

	/* Found it - now remove it from the list of entries: */
	*action_ptr = action->next;

	/* Currently used only by UML, might disappear one day: */
#ifdef CONFIG_IRQ_RELEASE_METHOD
	if (desc->irq_data.chip->release)
		desc->irq_data.chip->release(irq, dev_id);
#endif

	/* If this was the last handler, shut down the IRQ line: */
	if (!desc->action)
		irq_shutdown(desc);

	raw_spin_unlock_irqrestore(&desc->lock, flags);

	/* Make sure it's not being used on another CPU: */
	synchronize_irq(irq);

	return action;
}

void remove_irq(unsigned int irq, struct irqaction *act)
{
	__free_irq(irq, act->dev_id);
}
/************************************************************************/
/* EXPORT                                                               */
/************************************************************************/

int request_threaded_irq(unsigned int irq, irq_handler_t handler,
			 irq_handler_t thread_fn, unsigned long irqflags,
			 const char *devname, void *dev_id)
{
	struct irqaction *action;
	struct irq_desc *desc;
	int retval;

	/*
	 * Sanity-check: shared interrupts must pass in a real dev-ID,
	 * otherwise we'll have trouble later trying to figure out
	 * which interrupt is which (messes up the interrupt freeing
	 * logic etc).
	 */
	if ((irqflags & IRQF_SHARED) && !dev_id)
		return -EINVAL;
	desc = irq_to_desc(irq);
	if (!desc)
		return -EINVAL;

	if (!irq_settings_can_request(desc))
		return -EINVAL;

	if (!handler) {
		if (!thread_fn)
			return -EINVAL;
		handler = irq_default_primary_handler;
	}
	action = kzalloc(sizeof(struct irqaction), GFP_KERNEL);
	if (!action)
		return -ENOMEM;
	
	action->handler = handler;
	action->thread_fn = thread_fn;
	action->flags = irqflags;
	action->name = devname;
	action->dev_id = dev_id;

	chip_bus_lock(desc);
	retval = __setup_irq(irq, desc, action);
	chip_bus_sync_unlock(desc);

	if (retval)
		kfree(action);

	return retval;
}

int can_request_irq(unsigned int irq, unsigned long irqflags)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_lock(irq, &flags, 0);
	int canrequest = 0;

	if (!desc)
		return 0;

	if (irq_settings_can_request(desc)) {
		if (desc->action)
			if (irqflags & desc->action->flags & IRQF_SHARED)
				canrequest =1;
	}
	irq_put_desc_unlock(desc, flags);
	return canrequest;
}

void free_irq(unsigned int irq, void *dev_id)
{
	struct irq_desc *desc = irq_to_desc(irq);

	if (!desc)
		return;

	chip_bus_lock(desc);
	kfree(__free_irq(irq, dev_id));
	chip_bus_sync_unlock(desc);
}

void disable_irq(unsigned int irq)
{
	if (!__disable_irq_nosync(irq))
		synchronize_irq(irq);
}

void enable_irq(unsigned int irq)
{
	unsigned long flags;
	struct irq_desc *desc = irq_get_desc_buslock(irq, &flags, IRQ_GET_DESC_CHECK_GLOBAL);

	if (!desc)
		return;
	if (WARN(!desc->irq_data.chip,
		HAL_ERR "enable_irq before setup/request_irq: irq %u\n", irq))
		goto out;

	__enable_irq(desc, irq, false);
out:
	irq_put_desc_busunlock(desc, flags);
}

int setup_irq(unsigned int irq, struct irqaction *act)
{
	int retval;
	struct irq_desc *desc = irq_to_desc(irq);

	chip_bus_lock(desc);
	retval = __setup_irq(irq, desc, act);
	chip_bus_sync_unlock(desc);

	return retval;
}
