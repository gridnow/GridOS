#ifndef _ASM_X86_TIMEX_H
#define _ASM_X86_TIMEX_H


/* Assume we use the PIT time source for the clock tick */
#define CLOCK_TICK_RATE		PIT_TICK_RATE

/* Merged the original time.h */
#include <clocksource.h>
#include <asm/mc146818rtc.h>

extern void hpet_time_init(void);
extern void time_init(void);

extern struct clock_event_device *global_clock_event;


#endif /* _ASM_X86_TIMEX_H */
