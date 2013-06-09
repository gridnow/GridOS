/**
    @defgroup memory
    @ingroup kernel_api

    内核编程接口 内存操作方法

    @{
*/

#ifndef KE_MEMORY_H
#define KE_MEMORY_H

#include <compiler.h>
typedef unsigned int page_prot_t;
#define KM_PROT_READ	(1 << 0)
#define KM_PROT_WRITE	(1 << 1)
#define KM_MAP_DEVICE	(1 << 16)

/**
	@brief Map IO memory space
 */
DLLEXPORT void *km_map_physical(unsigned long physical, size_t size, unsigned int flags);

/**
	@brief Allocate a kernel page
*/
DLLEXPORT void *km_alloc_kernel_page();
#endif

/** @} */
