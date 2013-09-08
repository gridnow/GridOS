/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Jerry,wuxin
 *   
 */

#include "string.h"
#include "bitmap.h"
#include "bits.h"

#include "i2p.h"

struct i2p
{
	struct i2p_node *first;									//指向链表中第一个i2p node
	struct i2p_node *last;									//指向链表中最后一个i2p node
	int node_num;											//链表中i2p node 数
	unsigned long used_index;								//链表中总的已使用的index数
	unsigned long total_index;								//链表中总的可用的index数
	int max_index_per_node;									//i2p node中最大index数
	int next_index;											/* 下一索引描述符起始查找位置,
																释放索引时，比较释放索引值和该值，若释放索引值比该值小，将该值更新为释放的索引值；
																分配索引时，仅将该值+1。所以该值并不一定指向空闲描述符，仅指向空闲描述符开始查找的位置。
															*/
	i2p_free free_handle;									//i2p中释放内存的free函数
	i2p_malloc malloc_handle;								//i2p中分配内存的malloc函数
};

struct i2p_node
{
	int count;												//该node下index使用计数
	unsigned long start_index;								//该i2p_node块中起始的索引号
	unsigned long end_index;								//该i2p_node块中终止的索引号
	struct cl_bitmap arr_bitmap;
	struct i2p_node *pre;									//指向链表中上一个i2p node.
	struct i2p_node *next;									//指向链表中下一个i2p node.
	struct i2p *head;										//指向链表头结点

	void ** array;
};

void print_i2p_node(struct i2p_node * i2pt)
{
	if(!i2pt)
		return;

	printk("i2p node: %p\n",i2pt);
	printk("  count:  %d",i2pt->count);
	printk("  start_index:  %d",i2pt->start_index);
	printk("  end_index:  %d\n",i2pt->end_index);
	printk("  pre:  %p",i2pt->pre);
	printk("  next:  %p",i2pt->next);
	printk("  head:  %p",i2pt->head);
	printk("  array:  %p\n",i2pt->array);
	printk("  bitmap.last_bi:  %p\n",i2pt->arr_bitmap.last_bit);
	printk("  bitmap.mask_count:  %p\n",i2pt->arr_bitmap.mask_count);

	return;
}

void print_i2p(struct i2p * i2pt_head)
{
	if(!i2pt_head)
		return;

	printk("i2p head: %p\n",i2pt_head);
	printk("  first:  %p",i2pt_head->first);
	printk("  last:  %p",i2pt_head->last);
	printk("  node num:  %d\n",i2pt_head->node_num);
	printk("  used_index:  %d",i2pt_head->used_index);
	printk("  total_index:  %d\n",i2pt_head->total_index);
	printk("  max_index_per_node:  %d",i2pt_head->max_index_per_node);
	printk("  next_index:  %d\n",i2pt_head->next_index);

	return;
}


static struct i2p_node *i2p_node_create(struct i2p *i2pt_head)
{
	struct i2p_node *i2pt = NULL;
	unsigned long *bitmap_buf = NULL;
	int node_size, bitmap_bufsize;

	bitmap_bufsize	= MAX_COUNT_PER_ARRAY / CHAR_BIT + 1;					// (MAX_COUNT_PER_ARRAY/CHAR_BIT)向上取整
	node_size = sizeof(struct i2p_node) + MAX_ARRAY_SIZE + bitmap_bufsize;

	i2pt = i2pt_head->malloc_handle(node_size);	
//	printk("sizeof(struct i2p_node)=%d, arraysize=%d, bitmap_bufsize=%d, nodesize=%d\n", sizeof(struct i2p_node), MAX_ARRAY_SIZE, bitmap_bufsize, node_size);
//	printk("i2pt=%p\n", i2pt);

	if (!i2pt)
		goto err1;
	memset(i2pt, 0, node_size);

	i2pt->array				= (void *)i2pt + sizeof(struct i2p_node);
	bitmap_buf				= (unsigned long*)((unsigned char *)i2pt + sizeof(struct i2p_node) + MAX_ARRAY_SIZE);
	i2pt->pre 				= i2pt_head->last;
	i2pt->start_index		= i2pt->pre ? (i2pt->pre->end_index + 1) : 0;/*第一个i2p node index 范围从0 -- max_index_per_node-1*/
	i2pt->end_index			= i2pt->pre ? (i2pt->pre->end_index + i2pt_head->max_index_per_node) : i2pt_head->max_index_per_node - 1;
	i2pt->count 			= 0;
	i2pt->next 				= NULL;
	i2pt->head				= i2pt_head;
	cl_bitmap_init(&i2pt->arr_bitmap, bitmap_buf, MAX_COUNT_PER_ARRAY);

	/*
		如果链表中已有结点，则将新建结点链接到链表尾
	*/
	if(i2pt_head->last)
		i2pt_head->last->next = i2pt;

	i2pt_head->last			= i2pt;
	i2pt_head->node_num	++;		
	i2pt_head->total_index	+= i2pt_head->max_index_per_node;

	return i2pt;

err1:
	return NULL;
}

/* delete the empty nodes at the rear of list, reserve one node at least */
static void __i2p_node_release(struct i2p *i2pt_head)
{
	struct i2p_node *i2pt = i2pt_head->last;

	/* 
		从链表尾部开始遍历链表，当遍历到的结点没有使用时释放该结点的所占空间
		当遇到有非空闲索引的结点时，循环退出
		给链表至少保留一个结点，方便后续分配索引
	*/
	while (i2pt && i2pt->count == 0 && i2pt_head->node_num != 1)
	{
		i2pt->pre->next = NULL;
	
		i2pt_head->last	= i2pt->pre;
		i2pt_head->free_handle(i2pt);
		i2pt_head->node_num -- ;
		i2pt_head->total_index -= i2pt_head->max_index_per_node;

		i2pt			= i2pt_head->last;
	}
}

/* 释放i2p对象中的一个索引 */
static bool __i2p_dealloc(struct i2p *i2pt_head, i2p_handle index)
{
	int ret;
	i2p_handle offset;
	struct i2p_node *i2pt = i2pt_head->first;

	/* 定位索引所在结点 */
	while (index > i2pt->end_index)
	{
		if (i2pt->next)
			i2pt = i2pt->next;	
		else
			goto err_out;
	}

	offset = index - i2pt->start_index;							// 计算释放的索引描述符在结点中的索引偏移
	memset(i2pt->array + offset, 0, sizeof(void *));

	ret = cl_bitmap_dealloc_bit(&i2pt->arr_bitmap, offset);		// 释放索引对应的bitmap位
	if(ret == false)
		goto err_out;
	
	i2pt_head->used_index --;

	i2pt->count--;
	if (index < i2pt->head->next_index)							// 若释放的索引小于头结点的next_index,将索引值赋给next_index
		i2pt->head->next_index = index;

	if (i2pt->count == 0 && i2pt == i2pt->head->last			// 若释放索引所在结点使用计数为0且为链表尾结点，调用结点释放函数
		&& i2pt->head->node_num != 1/*使i2p 链表至少保留一个node*/)  
	{
		__i2p_node_release(i2pt->head);
	}

	return true;

err_out:
	return false;
}

static i2p_handle __i2p_alloc(struct i2p *i2pt_head, void *ptr)
{
	int offset;
	i2p_handle index		= COMMON_I2P_ALLOC_ERROR;
	struct i2p_node *i2pt	= NULL;

	// no valid index in this i2p list，create a new  node and link the node to the rear of list.
	if (i2pt_head->used_index == i2pt_head->total_index)
	{
		//printk("i2pt_head->node_num=%d,now create a new node.\n",i2pt_head->node_num);
		i2pt		= i2p_node_create(i2pt_head);
		if (i2pt == NULL)
			return index;
	}

	// 遍历链表，定位next_index所在结点
	i2pt = i2pt_head->first;
	while (i2pt_head->next_index > i2pt->end_index || i2pt->count == i2pt_head->max_index_per_node/* 该结点已满 */)
	{
//		printk("i2pt_head->next_index=%d, i2pt->end_index=%d\n",i2pt_head->next_index,i2pt->end_index);
		i2pt = i2pt->next;	
	}
	offset = cl_bitmap_alloc_bit(&i2pt->arr_bitmap);
	//	printk("offset=%d\n",offset);

	// 定位可用的下一空闲索引描述符，参考next_index定义
	while (offset == HAL_BITMAP_ALLOC_BIT_ERROR)
	{
		i2pt = i2pt->next;		
		offset = cl_bitmap_alloc_bit(&i2pt->arr_bitmap);
	}

	index = offset + i2pt->start_index;
	i2pt->count ++;
	memcpy(i2pt->array + offset, &ptr, sizeof(void *));					// 将资源指针填充到资源描述符转换表中对应索引处

	i2pt_head->used_index ++;
	i2pt_head->next_index = index + 1;									// 更新索引描述符查找起始位置

	return index;
}

static void * __i2p_find(struct i2p *i2pt_head, i2p_handle index)
{
	struct i2p_node *i2pt = i2pt_head->first;

	/* 定位索引所在结点 */
	while (index > i2pt->end_index)
	{
		if (i2pt->next)
			i2pt = i2pt->next;
		else
			return NULL;
	}

	return (void *) (*(i2pt->array + (index - i2pt->start_index)));				
}

bool i2p_dealloc(struct i2p *i2pt_head, i2p_handle index)
{
	return __i2p_dealloc(i2pt_head, index);
}


void *i2p_find(struct i2p *i2pt_head, i2p_handle index)
{
	return __i2p_find(i2pt_head, index);
}

unsigned long i2p_loop(struct i2p *i2p_list, void(*action)(void *process, i2p_handle handle), void *process)
{
	int ret = 0;
	i2p_handle offset;
	struct i2p_node *i2pt;

	i2pt = i2p_list->first;

	/* 遍历链表，查找每一个i2p索引 */
	for (ret = 0; ret < i2p_list->used_index; ret++)
	{
		offset = cl_find_next_bit(i2pt->arr_bitmap.bitmap, i2pt->arr_bitmap.mask_count, offset);
		
		/* 是否遍历完当前i2p节点，若是，则查找链表中的下一个i2p节点 */
		if (offset == i2pt->arr_bitmap.mask_count)															
		{
			i2pt = i2pt->next;
			if (i2pt == NULL)
				break;

			offset = 0;
			offset = cl_find_next_bit(i2pt->arr_bitmap.bitmap, i2pt->arr_bitmap.mask_count, offset);
		}

		action(process, offset);
		offset += 1;
	}	
	
	return ret;
}

void i2p_delete(struct i2p *i2pt_head)
{
	bool ret;
	struct i2p_node *i2pt;

	//  hal_spin_lock(i2pt->spin_lock);
	i2pt = i2pt_head->first;
	while (i2pt)
	{
		i2pt_head->first = i2pt->next;
		i2pt_head->free_handle(i2pt);
		i2pt = i2pt_head->first;
	}
	//	hal_spin_unlock(i2pt->spin_lock);
	i2pt_head->free_handle(i2pt_head);

	return ;
}

i2p_handle i2p_alloc(struct i2p *i2pt_head, void *ptr)
{
	return __i2p_alloc(i2pt_head, ptr);
}

struct i2p *i2p_create(i2p_malloc malloc_handle, i2p_free free_handle)
{
	struct i2p_node *i2pt;
	struct i2p *i2pt_head;
	void *bitmap_buf;

	i2pt_head = malloc_handle(sizeof(struct i2p));
	if (!i2pt_head)
		goto err0;

	memset(i2pt_head, 0, sizeof(struct i2p));

	i2pt_head->max_index_per_node	= MAX_COUNT_PER_ARRAY;
	i2pt_head->free_handle			= free_handle;
	i2pt_head->malloc_handle		= malloc_handle;
	i2pt							= i2p_node_create(i2pt_head);
	if(i2pt == NULL)
		goto err1;

	i2pt_head->first				= i2pt;

	return i2pt_head;

err1:
	free_handle(i2pt_head);
err0:
	return NULL;
}