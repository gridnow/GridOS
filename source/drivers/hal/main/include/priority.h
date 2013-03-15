/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程优先级描述
*/

#ifndef KT_THREAD_PRIORITY_H
#define KT_THREAD_PRIORITY_H
#include <types.h>

/* Thread priority */
#define KT_PRIORITY_HIGHEST			0
#define KT_PRIORITY_NORMAL			(BITS_PER_LONG / 2)
#define KT_PRIORITY_LOWEST			(BITS_PER_LONG - 1)
#define KT_PRIORITY_COUNT			(BITS_PER_LONG)

#endif

