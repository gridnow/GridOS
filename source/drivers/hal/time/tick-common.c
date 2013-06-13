#include <clockchips.h>
#include <timer.h>

void tick_handle_periodic(struct clock_event_device *dev)
{
	run_local_timers();
}