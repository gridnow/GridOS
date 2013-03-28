#include <clockchips.h>
#include <spinlock.h>
#include <debug.h>
#include <limits.h>

#include <asm/div64.h>

static LIST_HEAD(clockevent_devices);
static DEFINE_RAW_SPINLOCK(clockevents_lock);

u64 clockevent_delta2ns(unsigned long latch, struct clock_event_device *evt)
{
	u64 clc = (u64) latch << evt->shift;

	if (unlikely(!evt->mult)) {
		evt->mult = 1;
		WARN_ON(1);
	}

	do_div(clc, evt->mult);
	if (clc < 1000)
		clc = 1000;
	if (clc > KTIME_MAX)
		clc = KTIME_MAX;

	return clc;
}

void clockevents_set_mode(struct clock_event_device *dev,
				 enum clock_event_mode mode)
{
	if (dev->mode != mode) {
		dev->set_mode(mode, dev);
		dev->mode = mode;

		/*
		 * A nsec2cyc multiplicator of 0 is invalid and we'd crash
		 * on it, so fix it up and emit a warning:
		 */
		if (mode == CLOCK_EVT_MODE_ONESHOT) {
			if (unlikely(!dev->mult)) {
				dev->mult = 1;
				WARN_ON(1);
			}
		}
	}
}

void clockevents_register_device(struct clock_event_device *dev)
{
	unsigned long flags;

	raw_spin_lock_irqsave(&clockevents_lock, flags);

	list_add(&dev->list, &clockevent_devices);

	raw_spin_unlock_irqrestore(&clockevents_lock, flags);
}

void clockevents_config(struct clock_event_device *dev, u32 freq)
{
	u64 sec;

	if (!(dev->features & CLOCK_EVT_FEAT_ONESHOT))
		return;

	/*
	 * Calculate the maximum number of seconds we can sleep. Limit
	 * to 10 minutes for hardware which can program more than
	 * 32bit ticks so we still get reasonable conversion values.
	 */
	sec = dev->max_delta_ticks;
	do_div(sec, freq);
	if (!sec)
		sec = 1;
	else if (sec > 600 && dev->max_delta_ticks > UINT_MAX)
		sec = 600;

	clockevents_calc_mult_shift(dev, freq, sec);
	dev->min_delta_ns = clockevent_delta2ns(dev->min_delta_ticks, dev);
	dev->max_delta_ns = clockevent_delta2ns(dev->max_delta_ticks, dev);

}

/* 
	@min_delta:	The minimum clock ticks to program in oneshot mode
	@max_delta:	The maximum clock ticks to program in oneshot mode
*/
void clockevents_config_and_register(struct clock_event_device *dev,
				     u32 freq, unsigned long min_delta,
				     unsigned long max_delta)
{
	dev->min_delta_ticks = min_delta;
	dev->max_delta_ticks = max_delta;
	clockevents_config(dev, freq);
	clockevents_register_device(dev);
}


