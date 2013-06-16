#ifndef HAL_TIMER_H
#define HAL_TIMER_H

void __init hal_timer_system_init(void);
extern void run_local_timers(void);

#endif