#ifndef HAL_I8253_H
#define HAL_I8253_H

#include <param.h>
#include <lock.h>
#include <ddk/compatible.h>

/* i8253A PIT registers */
#define PIT_MODE	0x43
#define PIT_CH0		0x40
#define PIT_CH2		0x42

/* The clock frequency of the i8253/i8254 PIT */
#define PIT_TICK_RATE 1193182ul
#define PIT_LATCH	((PIT_TICK_RATE + HZ/2) / HZ)

extern raw_spinlock_t i8253_lock;
extern struct clock_event_device i8253_clockevent;
extern void clockevent_i8253_init(bool oneshot);

extern void setup_pit_timer(void);

#endif
