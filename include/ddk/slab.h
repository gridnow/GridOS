/**
*  @defgroup MemoryAllocatorForDriver
*  @ingroup DDK
*
*  定义了驱动程序要调用的内存分配器接口
*  @{
*/

#ifndef _DDK_SLAB_H_
#define _DDK_SLAB_H_

#include <ddk/types.h>
#include <ddk/string.h>

/* Compatible flags */
#define GFP_KERNEL (1 << 0)
#define GFP_ZERO (1 << 1)
#define __GFP_ZERO GFP_ZERO
#define gfp_t u32

/* Memory allocation type */
void *hal_malloc(int size);
void *kmalloc_node(int size, unsigned long unused, int cpu_node);
void hal_free(void * hal_address);

/*
	传统驱动直接用的接口，我们要对其转换
*/
#define kzalloc(__size__, __flags__)	\
({ \
	void * p = hal_malloc(__size__);	\
	memset(p, 0, __size__);	\
	p; \
})

/* 兼容接口 */
static inline void *kmalloc_track_caller(size_t size, unsigned int flags)
{
	void *p = hal_malloc(size);
	if (flags & GFP_ZERO)
		memset(p, 0, size);
	return p;
}

static inline void *kcalloc(size_t n, size_t size, unsigned int flags)
{
	return kzalloc(n * size, flags);
}

static inline void *kmalloc(size_t size, unsigned int flags)
{
	return kmalloc_track_caller(size, flags);
}

#define kfree hal_free

#endif

/** @} */
