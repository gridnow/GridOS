/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
    @defgroup atomic
    @ingroup kernel_api

    内核编程接口 原子计数器

    @{
*/

#ifndef KE_ATOMIC_H
#define KE_ATOMIC_H

#include <compiler.h>

struct ke_atomic
{
	int counter;
};

static inline int ke_atomic_read(struct ke_atomic * atomic)
{
	return (*(volatile int *)&(atomic)->counter);
}
DLLEXPORT void ke_atomic_set(struct ke_atomic * atomic, int count);
DLLEXPORT void ke_atomic_inc(struct ke_atomic * atomic);
DLLEXPORT int ke_atomic_sub_return(struct ke_atomic * atomic, int count);
DLLEXPORT int ke_atomic_add_return(struct ke_atomic * atomic, int count);
DLLEXPORT void ke_atomic_dec(struct ke_atomic * atomic);
#endif

/** @} */
