/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 时间管理
*/

#include <types.h>
#include <time.h>
#include <clockchips.h>
#include <tick.h>
#include <timer.h>

#include <asm/timex.h>

void (*__initdata late_time_init)(void);

unsigned long msecs_to_jiffies(const unsigned int m)
{
	/*
	 * Negative value, means infinite timeout:
	 */
	if ((int)m < 0)
		return MAX_JIFFY_OFFSET;
	
#if HZ <= MSEC_PER_SEC && !(MSEC_PER_SEC % HZ)
	/*
	 * HZ is equal to or smaller than 1000, and 1000 is a nice
	 * round multiple of HZ, divide with the factor between them,
	 * but round upwards:
	 */
	return (m + (MSEC_PER_SEC / HZ) - 1) / (MSEC_PER_SEC / HZ);
#elif HZ > MSEC_PER_SEC && !(HZ % MSEC_PER_SEC)
	/*
	 * HZ is larger than 1000, and HZ is a nice round multiple of
	 * 1000 - simply multiply with the factor between them.
	 *
	 * But first make sure the multiplication result cannot
	 * overflow:
	 */
	if (m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
	
	return m * (HZ / MSEC_PER_SEC);
#else
	/*
	 * Generic case - multiply, round and divide. But first
	 * check that if we are doing a net multiplication, that
	 * we wouldn't overflow:
	 */
	if (HZ > MSEC_PER_SEC && m > jiffies_to_msecs(MAX_JIFFY_OFFSET))
		return MAX_JIFFY_OFFSET;
	
	return (MSEC_TO_HZ_MUL32 * m + MSEC_TO_HZ_ADJ32)
	>> MSEC_TO_HZ_SHR32;
#endif
}

/*********************************************************************
 Export interface
*********************************************************************/
unsigned long hal_get_tick()
{
	return jiffies;
}

unsigned int hal_get_tick_rate()
{
	return HZ;
}

unsigned long hal_ms_to_tick(int ms)
{
	return msecs_to_jiffies(ms);
}

void hal_time_init()
{
	/* 软件定时器 */
	hal_timer_init();
	
	/* 硬件设备 */
	rtc_init();
	if (late_time_init)
		late_time_init();
}

