#ifndef X86_TSC_H
#define X86_TSC_H
#include "processor.h"
#include "msr.h"
#include "div64.h"

#define NS_SCALE	10 /* 2^10, carefully chosen */
#define US_SCALE	32 /* 2^32, arbitralrily chosen */

/*
* Standard way to access the cycle counter.
*/
typedef unsigned long long cycles_t;

extern unsigned int cpu_khz;
extern unsigned int tsc_khz;

static inline cycles_t get_cycles(void)
{
	unsigned long long ret = 0;

#ifndef CONFIG_X86_TSC
	if (!cpu_has_tsc)
		return 0;
#endif
	rdtscll(ret);

	return ret;
}

/* private functions */
void __init tsc_init(void);
int check_tsc_unstable(void);
unsigned long native_calibrate_tsc(void);
#endif

