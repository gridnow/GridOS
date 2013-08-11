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
DLLEXPORT void *km_map_physical(unsigned long physical, size_t size, unsigned long flags);
#define KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL	(1 << 0)				/* 物理地址绑定到特定虚拟地址 */
#define KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE	(1 << 1)				/* 物理地址映射时，使用常规的cache模式（而不是非缓存）*/

/**
	@brief Allocate a kernel page
*/
DLLEXPORT void *km_alloc_kernel_page();
#endif

/** @} */
