/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Jerry,wuxin
 *   
 */


#include "bitmap.h"
#include "bits.h"
#include "cl_string.h"

#include "i2p.h"

struct i2p
{
	struct i2p_node *first;									//ָ�������е�һ��i2p node
	struct i2p_node *last;									//ָ�����������һ��i2p node
	int node_num;											//������i2p node ��
	unsigned long used_index;								//�������ܵ���ʹ�õ�index��
	unsigned long total_index;								//�������ܵĿ��õ�index��
	int max_index_per_node;									//i2p node�����index��
	int next_index;											/* ��һ������������ʼ����λ��,
																�ͷ�����ʱ���Ƚ��ͷ�����ֵ�͸�ֵ�����ͷ�����ֵ�ȸ�ֵС������ֵ����Ϊ�ͷŵ�����ֵ��
																��������ʱ��������ֵ+1�����Ը�ֵ����һ��ָ���������������ָ�������������ʼ���ҵ�λ�á�
															*/
	i2p_free free_handle;									//i2p���ͷ��ڴ��free����
	i2p_malloc malloc_handle;								//i2p�з����ڴ��malloc����
};

struct i2p_node
{
	int count;												//��node��indexʹ�ü���
	unsigned long start_index;								//��i2p_node������ʼ��������
	unsigned long end_index;								//��i2p_node������ֹ��������
	struct cl_bitmap arr_bitmap;
	struct i2p_node *pre;									//ָ����������һ��i2p node.
	struct i2p_node *next;									//ָ����������һ��i2p node.
	struct i2p *head;										//ָ������ͷ���

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

	bitmap_bufsize	= MAX_COUNT_PER_ARRAY / CHAR_BIT + 1;					// (MAX_COUNT_PER_ARRAY/CHAR_BIT)����ȡ��
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
	i2pt->start_index		= i2pt->pre ? (i2pt->pre->end_index + 1) : 0;/*��һ��i2p node index ��Χ��0 -- max_index_per_node-1*/
	i2pt->end_index			= i2pt->pre ? (i2pt->pre->end_index + i2pt_head->max_index_per_node) : i2pt_head->max_index_per_node - 1;
	i2pt->count 			= 0;
	i2pt->next 				= NULL;
	i2pt->head				= i2pt_head;
	cl_bitmap_init(&i2pt->arr_bitmap, bitmap_buf, MAX_COUNT_PER_ARRAY);

	/*
		������������н�㣬���½�������ӵ�����β
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
		������β����ʼ�����������������Ľ��û��ʹ��ʱ�ͷŸý�����ռ�ռ�
		�������зǿ��������Ľ��ʱ��ѭ���˳�
		���������ٱ���һ����㣬���������������
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

/* �ͷ�i2p�����е�һ������ */
static bool __i2p_dealloc(struct i2p *i2pt_head, i2p_handle index)
{
	int ret;
	i2p_handle offset;
	struct i2p_node *i2pt = i2pt_head->first;

	/* ��λ�������ڽ�� */
	while (index > i2pt->end_index)
	{
		if (i2pt->next)
			i2pt = i2pt->next;	
		else
			goto err_out;
	}

	offset = index - i2pt->start_index;							// �����ͷŵ������������ڽ���е�����ƫ��
	memset(i2pt->array + offset, 0, sizeof(void *));

	ret = cl_bitmap_dealloc_bit(&i2pt->arr_bitmap, offset);		// �ͷ�������Ӧ��bitmapλ
	if(ret == false)
		goto err_out;
	
	i2pt_head->used_index --;

	i2pt->count--;
	if (index < i2pt->head->next_index)							// ���ͷŵ�����С��ͷ����next_index,������ֵ����next_index
		i2pt->head->next_index = index;

	if (i2pt->count == 0 && i2pt == i2pt->head->last			// ���ͷ��������ڽ��ʹ�ü���Ϊ0��Ϊ����β��㣬���ý���ͷź���
		&& i2pt->head->node_num != 1/*ʹi2p �������ٱ���һ��node*/)  
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

	// no valid index in this i2p list��create a new  node and link the node to the rear of list.
	if (i2pt_head->used_index == i2pt_head->total_index)
	{
		//printk("i2pt_head->node_num=%d,now create a new node.\n",i2pt_head->node_num);
		i2pt		= i2p_node_create(i2pt_head);
		if (i2pt == NULL)
			return index;
	}

	// ����������λnext_index���ڽ��
	i2pt = i2pt_head->first;
	while (i2pt_head->next_index > i2pt->end_index || i2pt->count == i2pt_head->max_index_per_node/* �ý������ */)
	{
//		printk("i2pt_head->next_index=%d, i2pt->end_index=%d\n",i2pt_head->next_index,i2pt->end_index);
		i2pt = i2pt->next;	
	}
	offset = cl_bitmap_alloc_bit(&i2pt->arr_bitmap);
	//	printk("offset=%d\n",offset);

	// ��λ���õ���һ�����������������ο�next_index����
	while (offset == HAL_BITMAP_ALLOC_BIT_ERROR)
	{
		i2pt = i2pt->next;		
		offset = cl_bitmap_alloc_bit(&i2pt->arr_bitmap);
	}

	index = offset + i2pt->start_index;
	i2pt->count ++;
	memcpy(i2pt->array + offset, &ptr, sizeof(void *));					// ����Դָ����䵽��Դ������ת�����ж�Ӧ������

	i2pt_head->used_index ++;
	i2pt_head->next_index = index + 1;									// ��������������������ʼλ��

	return index;
}

static void * __i2p_find(struct i2p *i2pt_head, i2p_handle index)
{
	struct i2p_node *i2pt = i2pt_head->first;

	/* ��λ�������ڽ�� */
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

	/* ������������ÿһ��i2p���� */
	for (ret = 0; ret < i2p_list->used_index; ret++)
	{
		offset = cl_find_next_bit(i2pt->arr_bitmap.bitmap, i2pt->arr_bitmap.mask_count, offset);
		
		/* �Ƿ�����굱ǰi2p�ڵ㣬���ǣ�����������е���һ��i2p�ڵ� */
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