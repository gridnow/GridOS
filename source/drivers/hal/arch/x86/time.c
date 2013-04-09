#include <clockchips.h>
#include <i8253.h>
#include <time.h>
#include <irq.h>

#include <asm/timex.h>
#include <asm/x86_init.h>
#include <asm/tsc.h>

/*
 * Default timer interrupt handler for PIT/HPET
 */
static irqreturn_t timer_interrupt(int irq, void *dev_id)
{
	if (global_clock_event->event_handler == NULL)
	{
		return IRQ_NONE;
	}
	
	global_clock_event->event_handler(global_clock_event);
	return IRQ_HANDLED;
}

static struct irqaction irq0  = {
	.handler = timer_interrupt,
	.flags = IRQF_DISABLED | IRQF_NOBALANCING | IRQF_IRQPOLL | IRQF_TIMER,
	.name = "timer"
};

void __init setup_default_timer_irq(void)
{
	setup_irq(0, &irq0);
}

#define hpet_enable() 0

/* Default timer init function */
void __init hpet_time_init(void)
{
	if (!hpet_enable())
		setup_pit_timer();
	setup_default_timer_irq();
}

static __init void x86_late_time_init(void)
{
	x86_init.timers.timer_init();
	//tsc_init();
	//TODO: soupport TSC
	printk("%s->%s->%d : TODO to support TSC\n.",__FILE__,__FUNCTION__,__LINE__);
}

/*
 * Initialize TSC and delay the periodic timer init to
 * late x86_late_time_init() so ioremap works.
 */
void __init time_init(void)
{
	late_time_init = x86_late_time_init;
}
