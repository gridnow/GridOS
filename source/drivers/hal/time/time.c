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

#include <asm/timex.h>

void (*__initdata late_time_init)(void);

void hal_time_init()
{
	rtc_init();
	late_time_init();
}

