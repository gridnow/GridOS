
#include "ring_buff.h"
#include <string.h>
#include <stdio.h>
#include <stddef.h>
#define DEBUG_HEAD_INFO(head) \
	printf("=======================\n"\
			"head prev offset %d\n"\
			"head next offset %d\n"\
			"head valid flage %d\n"\
			"head pos size %d\n"\
			"head pos offset %d\n"\
			"=====================\n",\
			head->prev_head, head->next_head, head->valid_flag,\
			head->package_size, head->package_offset)
			

#define req_len_is_gt_package_len(curr_head, length) \
	((curr_head)->package_size < length + sizeof(struct ring_package))

#define cache_read_pos(cache) \
	((struct ring_package *)(cache->curr_read_pos + (void *)cache))

#define cache_write_pos(cache) \
	((struct ring_package *)((void *)cache + cache->curr_write_pos ))


#define buff_to_ring_package(buff) \
	((struct ring_package *)((void *)(buff) - sizeof(struct ring_package)))

/* 下面宏用于将缓冲区内的偏移量和指针直接的转换 */
#define ptr_to_cache_offset(ptr) ((unsigned long)(ptr) - (unsigned long)cache)

#define cache_offset_to_ptr(offset) ((void *)cache + (offset))

/* get ring package next and prev package */
#define get_next_ring_package(package) ((struct ring_package *)cache_offset_to_ptr(package->next_head))
#define get_prev_ring_package(package) ((struct ring_package *)cache_offset_to_ptr(package->prev_head))

/* 用于分割ring package头部计算另一个头部信息 */
#define calculate_new_package_size(old_package,req_len) \
	((old_package->package_size) - (req_len) - sizeof(struct ring_package))

#define calculate_new_package_offset(old_package,req_len) \
	(ptr_to_cache_offset(old_package) + (req_len) + 2 * sizeof(struct ring_package))

/* 获取缓冲区头部ring package */
#define cache_head_ring_package(cache) ((struct ring_package *)((void *)cache + sizeof(*(cache))))

/* 链接邻居ring package */
#define add_to_prev_head(head, new, cache) do{\
	((struct ring_package *)cache_offset_to_ptr(head->next_head))->prev_head = ptr_to_cache_offset(new);\
	new->next_head             = head->next_head;\
	new->prev_head             = ptr_to_cache_offset(head);\
	head->next_head            = ptr_to_cache_offset(new);\
	}while(0)

/* 循环缓冲区是否为空 */
#define cache_is_empty(cache) ((cache_write_pos(cache) == cache_read_pos(cache)) &&\
								(cache_write_pos(cache)->valid_flag == INVALID_FLAG))

#define cache_is_full(cache) ((cache_write_pos(cache) == cache_read_pos(cache)) &&\
								(cache_write_pos(cache)->valid_flag))

#define get_new_package_from_slice(slice, slice_len) \
	(cache_offset_to_ptr((ptr_to_cache_offset(slice)+ (slice_len) + sizeof(struct ring_package))))

#define slice_new_package_offset(slice) \
	(ptr_to_cache_offset((slice))+ sizeof(struct ring_package))


#define get_cache_head_by_ring_pkt(pkt) \
	((void *)(pkt) - (pkt)->package_offset + sizeof(struct ring_package))

/**
	@brief 给定一个内存区域,初始化为ring buff
	@return
		cache head 成功   NULL 失败
*/
struct ring_buff_cache * ring_buff_head_init(void *cache, size_t length)
{
	struct ring_buff_cache *buff = (struct ring_buff_cache *)cache;
	struct ring_package *first_head = NULL;
	
	/* init buff cache head */
	memset((void *)buff, 0, sizeof(struct ring_buff_cache));
	buff->is_init           = 1;
	buff->cache_buff_length = length;
	buff->curr_read_pos     = sizeof(*buff);
	buff->curr_write_pos    = sizeof(*buff);

	/* 初始化第一个空间buff */
	first_head = cache_head_ring_package(buff);
	memset(first_head, 0, sizeof(*first_head));
	first_head->next_head           = sizeof(struct ring_buff_cache);
	first_head->prev_head           = sizeof(struct ring_buff_cache);
	first_head->package_offset      = sizeof(struct ring_buff_cache) + sizeof(struct ring_package);
	first_head->package_size        = length - sizeof(struct ring_package) - sizeof(struct ring_buff_cache);
	
	return buff;
}

/*
	@brief 从空buff中分配一个新的buff
	
*/
static void *slice_buff(struct ring_buff_cache *cache, struct ring_package *slice_head, size_t length)
{
	struct ring_package *new_head = get_new_package_from_slice(slice_head, length);
	/*
		构造package head	
	*/
	new_head->valid_flag         = INVALID_FLAG;
	new_head->package_size       = calculate_new_package_size(slice_head, length);
	new_head->package_offset     = calculate_new_package_offset(slice_head, length);
	
	slice_head->package_size        = length;
	slice_head->package_offset      = slice_new_package_offset(slice_head);
	slice_head->valid_flag          = VALID_FLAG;

	/* add to prev head */
	add_to_prev_head(slice_head, new_head, cache);
	/* 调整下一个可用buff 空间 */
	cache->curr_write_pos = slice_head->next_head;

	return cache_offset_to_ptr(slice_head->package_offset);
}

/**
	@brief 分配一个报文头
	@return
		成功返回报文body  失败返回 NULL
*/
void *ring_buff_alloc(struct ring_buff_cache *cache, size_t length)
{
	struct ring_package *curr = NULL;
	int found = 0;

	/* length 必须sizeof(long)对齐,在某些嵌入式设备要求指针对齐 */
	length = ALIGN(length,sizeof(long));
	
	/* IS FULL ? */
	if (cache_is_full(cache))
		return NULL;
	
	curr = cache_write_pos(cache);

	/* 
		查找可用packge head
		当为空buff只需一次找到
	*/	
	do
	{
		/* 当前buff 是否有效*/
		if (curr->valid_flag)
		{
			curr = (struct ring_package *)cache_offset_to_ptr(curr->next_head);
			continue;
		}
		/* good!! 不用去分片空闲package了 */
		if (curr->package_size == length)
		{
			curr->valid_flag = VALID_FLAG;
			/* 调整下一个可写buff */
			cache->curr_write_pos = curr->next_head;

			return cache_offset_to_ptr(curr->package_offset);
		}
		
		/* 当前buff 大小是否满足 */
		if (!req_len_is_gt_package_len(curr, length))
		{
			found = 1;
			break;
		}

		curr = (struct ring_package *)cache_offset_to_ptr(curr->next_head);
	} while(curr != cache_read_pos(cache));

	/* 是否真的找到了可分配的buff */
	if (found)
		return slice_buff(cache, curr, length);

	return NULL;
}

/**
	@brief 释放buff 并合并相邻空闲buff
	@return 
		无
*/
void ring_buff_free(struct ring_buff_cache *cache, void *buff)
{
	/* 该释放地址对应的报文头 */
	struct ring_package *package;
	struct ring_package *prev = NULL, *next = NULL;
	int only_merge_prev = 0;
	
	package = buff_to_ring_package(buff);
	/* 报文头是否释放过 ? */
	if (!(package->valid_flag))
		return;
	
	package->valid_flag = INVALID_FLAG;
	
	/*
		释放的时候要获取报文的前后报文头,看是否有空闲报文可以合并
	*/
	prev = get_prev_ring_package(package);
	next = get_next_ring_package(package);

	/* 如果前面和后面队列一个处于队列头部和尾部 则这时只需合并一种情况。。。 */
	if (next == cache_head_ring_package(cache))
		only_merge_prev = 1;

	if (package == cache_head_ring_package(cache))
		goto mereg_next;
	
	/* 前面buff 可以合并 ? */
	if (!prev->valid_flag)
	{
		prev->next_head                           = package->next_head;
		get_next_ring_package(package)->prev_head = ptr_to_cache_offset(prev);
		prev->package_size                       += package->package_size + sizeof(*prev);

		/* 校正可读可写cache 位置 */
		if (package == cache_write_pos(cache))
			cache->curr_write_pos = ptr_to_cache_offset(prev);

		if (package == cache_read_pos(cache))
			cache->curr_read_pos  = ptr_to_cache_offset(prev);
		
		package = prev;
	}

	if (only_merge_prev)
		return;
	
mereg_next:
	/* 后面buff可以合并 ? */
	if (!next->valid_flag)
	{
		package->next_head                     = next->next_head;
		get_next_ring_package(next)->prev_head = ptr_to_cache_offset(package);
		package->package_size                 += next->package_size + sizeof(*next);

		/* 如果这时cache可写可读指针指向next 还要重新调整 */
		if (next == cache_write_pos(cache))
			cache->curr_write_pos = ptr_to_cache_offset(package);

		if (next == cache_read_pos(cache))
			cache->curr_read_pos = ptr_to_cache_offset(package);
	}

	return;
}


/**
	@brief 释放package 对象
	@return
		void
*/
void ring_buff_free_package(void *obj)
{
	struct ring_package *ring_pkt = (struct ring_package *)obj;
	struct ring_buff_cache *cache = (struct ring_buff_cache *)get_cache_head_by_ring_pkt(ring_pkt);
	return ring_buff_free(cache, (obj + sizeof(struct ring_package)));
}

/**
	@brief 获取当前可读取报文头
	@return
		有可读报文返回package  无返回 NULL
*/
struct ring_package *ring_cache_read_package(struct ring_buff_cache *cache)
{
	struct ring_package *curr_read, *next_package;

	/* 当前可读报文位置,由前一次read package的时候修正 */
	curr_read = cache_read_pos(cache);
	next_package = get_next_ring_package(curr_read);
		
	/* 修改下次可读取的报文头部位置 */
	while (ptr_to_cache_offset(next_package) != cache->curr_write_pos)
	{
		/* 是否为可读标志 */
		if (next_package->valid_flag == VALID_FLAG)
		{
			cache->curr_read_pos = ptr_to_cache_offset(next_package);
			goto out;
		}
			
		next_package = get_next_ring_package(next_package);
	}

	/* 
		oh,no 当前已经是可读的最后一个package head啦 
		既然没有找到合适的下一个可读ring package
		那么下一个可读ring package肯定应该是指向当前可写位置
	*/
	cache->curr_read_pos = cache->curr_write_pos;
	
out:
	/* 一般当前指定的是有效的 */
	if (curr_read->valid_flag == VALID_FLAG)
	{
		/* 当前package head 将被读取, 修改其状态为busy */
		curr_read->valid_flag = BUSY_FLAG;
		return curr_read;
	}
	
	return NULL;
}


/**
	@brief 调试使用,打印队列内所有的package信息
*/
void cache_package_head_info_debug(struct ring_buff_cache *cache)
{
	struct ring_package *next = cache_head_ring_package(cache);
	int head_count = 0;

	do
	{
		DEBUG_HEAD_INFO(next);
		head_count++;
		next = get_next_ring_package(next);
	} while(next != cache_head_ring_package(cache));
	
	printf("total package head %d\n", head_count);

	return;
}



