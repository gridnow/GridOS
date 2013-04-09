/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL CPU«¿’º≥ÈœÛ
*/
#ifndef HAL_PREEMPT_H
#define HAL_PREEMPT_H

#include <asm/thread.h>
#define hal_preempt_count()				(kt_arch_current()->preempt_count)
#define hal_preempt_check_resched()		do {/*TODO: to call scheduler */ } while (0)

#define hal_preempt_count_add(x)		do { hal_preempt_count() += (x); } while (0)
#define hal_preempt_count_sub(x)		do { hal_preempt_count() -= (x); } while (0)
#define hal_preempt_count_inc()			hal_preempt_count_add(1)
#define hal_preempt_count_dec()			hal_preempt_count_sub(1)

#define hal_preempt_disable() \
	do { \
		hal_preempt_count_inc(); \
		barrier(); \
	} while (0)

#define hal_preempt_enable() \
	do { \
		barrier(); \
		hal_preempt_count_dec(); \
		barrier(); \
		hal_preempt_check_resched(); \
	} while (0)

#define hal_preempt_enable_no_resched() \
	do { \
		barrier(); \
		hal_preempt_count_dec(); \
	} while (0)

#endif
