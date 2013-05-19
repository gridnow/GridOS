/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL原子计数器
*/

#ifndef HAL_ATOMIC_H
#define HAL_ATOMIC_H

#include <compiler.h>
#include <types.h>
typedef struct {
	int counter;
} atomic_t;

#include <asm/atomic.h>

#endif /* HAL_ATOMIC_H */

