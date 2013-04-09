#ifndef HAL_TIME_H
#define HAL_TIME_H

#include <types.h>

/* Parameters used to convert the timespec values: */
#define MSEC_PER_SEC	1000L
#define USEC_PER_MSEC	1000L
#define NSEC_PER_USEC	1000L
#define NSEC_PER_MSEC	1000000L
#define USEC_PER_SEC	1000000L
#define NSEC_PER_SEC	1000000000L
#define FSEC_PER_SEC	1000000000000000LL

extern void (*late_time_init)(void);

#define KTIME_MAX			((s64)~((u64)1 << 63))

#endif
