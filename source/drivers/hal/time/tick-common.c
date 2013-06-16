#include <clockchips.h>
#include <timer.h>
#include <tick.h>

void tick_handle_periodic(struct clock_event_device *dev)
{
	jiffies_64++;
	run_local_timers();
}