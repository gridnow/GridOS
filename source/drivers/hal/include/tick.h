#ifndef HAL_TICK_H
#define HAL_TICK_H

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

#endif
