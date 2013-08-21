#ifndef HAL_TICK_H
#define HAL_TICK_H

#include <limits.h>

#include <param.h>

#define __jiffy_data  __attribute__((section(".data")))

extern u64 __jiffy_data jiffies_64;
extern unsigned long volatile __jiffy_data jiffies;

#define time_after(a,b)		\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(b) - (long)(a) < 0))
#define time_before(a,b)	time_after(b,a)

#define time_after_eq(a,b)	\
	(typecheck(unsigned long, a) && \
	 typecheck(unsigned long, b) && \
	 ((long)(a) - (long)(b) >= 0))
#define time_before_eq(a,b)	time_after_eq(b,a)

#define MAX_JIFFY_OFFSET ((LONG_MAX >> 1)-1)
#define INITIAL_JIFFIES ((unsigned long)(unsigned int) (-300*HZ))

/*
 * Convert various time units to each other:
 */
extern unsigned long msecs_to_jiffies(const unsigned int m);

#endif
