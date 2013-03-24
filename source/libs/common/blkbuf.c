/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   固定尺寸的块分配器
*/
#include <types.h>
#include <string.h>

#include "blkbuf.h"
#include "bitmap.h"

struct block_instance
{
	struct block_instance *pre, *next;
	struct cl_bitmap bitmap;
	int		block_size;
	int		node_count;
	int		free_count;

	void	*call_back_parameter;
	cl_bkb_free_handler free_handler;
};
#define GET_DATA_AREA(bkb, BASE) ((unsigned long)(BASE) - (BASE)->node_count * (bkb)->node_size)
#define BLKBUF_ALLOCATE(DIRECTION, bkb)											\
	while (where)															\
	{																		\
		unsigned long id;													\
		id = cl_bitmap_alloc_bit(&where->bitmap);							\
		if (id != HAL_BITMAP_ALLOC_BIT_ERROR)								\
		{																	\
			p = (void*)(id * (bkb)->node_size/*Get the block offset*/ +		\
				GET_DATA_AREA(bkb, where)/*Get block base*/);				\
			goto got;														\
		}																	\
		where = where->DIRECTION;											\
	}																		\

/************************************************************************/
/*                                                                      */
/************************************************************************/

/**
	@brief 创建一个BLOCK数据缓冲区
*/
void cl_bkb_extend(struct cl_bkb *bkb, void *base, size_t size, cl_bkb_free_handler handler, void *para)
{
	struct block_instance *buf;
	size_t cur_count, bitmap_size; 
	size_t total_size;

	/* Get the count of node in block_size, and make room for bitmap */
	cur_count	= size / bkb->node_size;
	bitmap_size = cl_bitmap_calc_size(cur_count);
	
	while ((total_size = bitmap_size + cur_count * bkb->node_size + sizeof(struct cl_bkb)) > size)
	{
		cur_count--;
		bitmap_size = cl_bitmap_calc_size(cur_count);
	}

#if 1
	{
		printk("分配%s调整至%d个节点,每个%d大小,总尺寸%d,浪费%d字节,地址:%x.\n",
			bkb->name, 
			cur_count,
			bkb->node_size, 
			total_size,
			size - total_size,
			base);
	}
#endif
	
	/*
		Init an instance.
		Put the management data at the end of the buffer to hack the node alignment problem in FPU.
	*/	
	buf	= (void*)((unsigned long)base + cur_count * bkb->node_size);
	memset(buf, 0, sizeof(*buf));
	buf->call_back_parameter = para;
	buf->free_handler	= handler;
	buf->free_count		=
		buf->node_count	= cur_count;	
	buf->block_size		= size;
	cl_bitmap_init(&buf->bitmap, (unsigned long*)(buf + 1)/*bitmap area*/, cur_count);

	/* Insert to the instance list */
	if (bkb->prefer == NULL)
		bkb->prefer = buf;
	else
	{
		struct block_instance *p = bkb->prefer;
		buf->pre = p;
		buf->next = p->next;
		if (p->next)
			p->next->pre = buf;
		p->next = buf;		
	}
}

void *cl_bkb_alloc(struct cl_bkb *bkb)
{
	struct block_instance *where;
	void *p;

	/* Try all node of next */
	where = bkb->prefer;
	BLKBUF_ALLOCATE(next, bkb);

	/* Try all node of pre */
	where = bkb->prefer;
	if (where)
	{
		where = where->pre;
		BLKBUF_ALLOCATE(pre, bkb);
	}

	return NULL;
got:
	return p;
}
