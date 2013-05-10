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

DLLEXPORT int ke_atomic_sub_return(struct ke_atomic * atomic, int count);
DLLEXPORT int ke_atomic_add_return(struct ke_atomic * atomic, int count);
DLLEXPORT void ke_atomic_dec(struct ke_atomic * atomic);
#endif

/** @} */
