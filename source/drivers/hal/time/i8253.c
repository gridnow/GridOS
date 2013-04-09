/*
 * i8253 PIT clocksource
 */
#include <clockchips.h>
#include <spinlock.h>
#include <i8253.h>

#include <asm/io.h>

/*
 * Protects access to I/O ports
 *
 * 0040-0043 : timer0, i8253 / i8254
 * 0061-0061 : NMI Control Register which contains two speaker control bits.
 */
DEFINE_RAW_SPINLOCK(i8253_lock);

#ifdef CONFIG_CLKEVT_I8253
/*
 * Initialize the PIT timer.
 *
 * This is also called after resume to bring the PIT into operation again.
 */
static void init_pit_timer(enum clock_event_mode mode,
			   struct clock_event_device *evt)
{
	raw_spin_lock(&i8253_lock);

	switch (mode) {
	case CLOCK_EVT_MODE_PERIODIC:
		/* binary, mode 2, LSB/MSB, ch 0 */
		outb_p(0x34, PIT_MODE);
		outb_p(PIT_LATCH & 0xff , PIT_CH0);	/* LSB */
		outb_p(PIT_LATCH >> 8 , PIT_CH0);		/* MSB */
		break;

	case CLOCK_EVT_MODE_SHUTDOWN:
	case CLOCK_EVT_MODE_UNUSED:
		if (evt->mode == CLOCK_EVT_MODE_PERIODIC ||
		    evt->mode == CLOCK_EVT_MODE_ONESHOT) {
			outb_p(0x30, PIT_MODE);
			outb_p(0, PIT_CH0);
			outb_p(0, PIT_CH0);
		}
		break;

	case CLOCK_EVT_MODE_ONESHOT:
		/* One shot setup */
		outb_p(0x38, PIT_MODE);
		break;

	case CLOCK_EVT_MODE_RESUME:
		/* Nothing to do here */
		break;
	}
	raw_spin_unlock(&i8253_lock);
}

/*
 * Program the next event in oneshot mode
 *
 * Delta is given in PIT ticks
 */
static int pit_next_event(unsigned long delta, struct clock_event_device *evt)
{
	raw_spin_lock(&i8253_lock);
	outb_p(delta & 0xff , PIT_CH0);	/* LSB */
	outb_p(delta >> 8 , PIT_CH0);		/* MSB */
	raw_spin_unlock(&i8253_lock);

	return 0;
}

/*
 * On UP the PIT can serve all of the possible timer functions. On SMP systems
 * it can be solely used for the global tick.
 */
struct clock_event_device i8253_clockevent = {
	.name		= "pit",
	.features	= CLOCK_EVT_FEAT_PERIODIC,
	.set_mode	= init_pit_timer,
	.set_next_event = pit_next_event,
};

/*
 * Initialize the conversion factor and the min/max deltas of the clock event
 * structure and register the clock event source with the framework.
 */
void __init clockevent_i8253_init(bool oneshot)
{
	if (oneshot)
		i8253_clockevent.features |= CLOCK_EVT_FEAT_ONESHOT;

	clockevents_config_and_register(&i8253_clockevent, PIT_TICK_RATE,
					0xF, 0x7FFF);
}
#endif

