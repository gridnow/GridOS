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

#ifdef CONFIG_64BIT
typedef struct {
	long counter;
} atomic64_t;
#else
typedef struct {
	u64 __aligned(8) counter;
} atomic64_t;
#endif

#include <asm/atomic.h>

#include <asm-generic/atomic-long.h>

#endif /* HAL_ATOMIC_H */

