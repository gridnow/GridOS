/**
*  @defgroup MemoryAllocatorForDriver
*  @ingroup DDK
*
*  ��������������Ҫ���õ��ڴ�������ӿ�
*  @{
*/

#ifndef _DDK_SLAB_H_
#define _DDK_SLAB_H_

#include <ddk/string.h>

#define GFP_KERNEL (1 << 0)
#define GFP_ZERO (1 << 1)
#define gfp_t u32

/* Memory allocation type */
void * hal_malloc(int size);

void hal_free(void * hal_address);

/*
	��ͳ����ֱ���õĽӿڣ�����Ҫ����ת��
*/
#define kzalloc(__size__, __flags__)	\
({ \
	void * p = hal_malloc(__size__);	\
	memset(p, 0, __size__);	\
	p; \
})

/* ���ݽӿ� */
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
