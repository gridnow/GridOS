/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
    @defgroup memory
    @ingroup kernel_api

    内核编程接口 内存操作方法

    @{
*/

#ifndef KE_MEMORY_H
#define KE_MEMORY_H

#include <compiler.h>
typedef u64 phy_t;

/**
	@brief Map IO memory space
 */
DLLEXPORT void *km_map_physical(phy_t physical, size_t size, unsigned int flags);

#endif

/** @} */
