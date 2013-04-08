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

struct ke_atomic
{
	int counter;
};


#endif

/** @} */
