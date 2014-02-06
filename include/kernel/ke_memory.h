/**
    @defgroup memory
    @ingroup kernel_api

    内核编程接口 内存操作方法

    @{
*/

#ifndef KE_MEMORY_H
#define KE_MEMORY_H

#include <compiler.h>
#include <types.h>

#include "kernel.h"

/**
	@brief Map IO memory space
*/
DLLEXPORT void *km_map_physical(unsigned long physical, unsigned long size, unsigned long flags);
#define KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL	(1 << 0)				/* 物理地址绑定到特定虚拟地址, 虚拟地址必须PAGE_SIZE对齐 */
#define KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE	(1 << 1)				/* 物理地址映射时，使用常规的cache模式（而不是非缓存）*/

/**
	@brief 分配内核态全局常规内存空间段
*/
DLLEXPORT void *km_alloc_virtual(unsigned long size, page_prot_t prot, void **__out kernel_space_object);

/**
	@brief 释放内核全局常规内存空间段
*/
DLLEXPORT void km_dealloc_virtual(void *kv);

/**
	@brief Allocate a kernel page
*/
DLLEXPORT void *km_alloc_kernel_page();

/**
	@brief 分配内核态全局公共常规内存空间
*/
DLLEXPORT void *km_valloc(unsigned long size);

/**
	@brief 释放内核全局公共常规内存空间
*/
DLLEXPORT void *km_vfree(void *kp);

#endif

/** @} */
