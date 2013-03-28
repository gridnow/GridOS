#include <clockchips.h>
#include <i8253.h>

struct clock_event_device *global_clock_event;

void __init setup_pit_timer(void)
{
	clockevent_i8253_init(true);
	global_clock_event = &i8253_clockevent;
}

