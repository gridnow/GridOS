/**
*  @defgroup MemoryAllocatorForDriver
*  @ingroup DDK
*
*  ��������������Ҫ���õ��ڴ�������ӿ�
*  @{
*/

#ifndef _DDK_SLAB_H_
#define _DDK_SLAB_H_

#include <string.h>

#define GFP_KERNEL 0

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

static inline void *kcalloc(size_t n, size_t size, unsigned int flags)
{
	return kzalloc(n *size, flags);
}

#define kfree hal_free



#endif

/** @} */
