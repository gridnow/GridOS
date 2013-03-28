#ifndef HAL_CLOCKCHIP_H
#define HAL_CLOCKCHIP_H

#include <types.h>
#include <cache.h>
#include <time.h>
#include <list.h>

#include <clocksource.h>

/* Clock event mode commands */
enum clock_event_mode {
	CLOCK_EVT_MODE_UNUSED = 0,
	CLOCK_EVT_MODE_SHUTDOWN,
	CLOCK_EVT_MODE_PERIODIC,
	CLOCK_EVT_MODE_ONESHOT,
	CLOCK_EVT_MODE_RESUME,
};

/* Clock event features */
#define CLOCK_EVT_FEAT_PERIODIC		0x000001
#define CLOCK_EVT_FEAT_ONESHOT		0x000002
#define CLOCK_EVT_FEAT_KTIME		0x000004

struct clock_event_device {
	void					(*event_handler)(struct clock_event_device *);	
	int						(*set_next_event)(unsigned long evt, struct clock_event_device *);

	u64						max_delta_ns;
	u64						min_delta_ns;
	u32						mult;
	u32						shift;
	enum clock_event_mode	mode;
	unsigned int			features;
	void					(*set_mode)(enum clock_event_mode mode, struct clock_event_device *);

	unsigned long			min_delta_ticks;
	unsigned long			max_delta_ticks;
	const char				*name;
	struct list_head		list;
} ____cacheline_aligned;

static inline void
clockevents_calc_mult_shift(struct clock_event_device *ce, u32 freq, u32 minsec)
{
	return clocks_calc_mult_shift(&ce->mult, &ce->shift, NSEC_PER_SEC,
				      freq, minsec);
}

extern void clockevents_config_and_register(struct clock_event_device *dev,
					    u32 freq, unsigned long min_delta,
					    unsigned long max_delta);
extern void clockevents_set_mode(struct clock_event_device *dev,
				 enum clock_event_mode mode);

#endif

