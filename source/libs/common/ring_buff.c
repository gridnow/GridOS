
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

/* ��������ڽ��������ڵ�ƫ������ָ��ֱ�ӵ�ת�� */
#define ptr_to_cache_offset(ptr) ((unsigned long)(ptr) - (unsigned long)cache)

#define cache_offset_to_ptr(offset) ((void *)cache + (offset))

/* get ring package next and prev package */
#define get_next_ring_package(package) ((struct ring_package *)cache_offset_to_ptr(package->next_head))
#define get_prev_ring_package(package) ((struct ring_package *)cache_offset_to_ptr(package->prev_head))

/* ���ڷָ�ring packageͷ��������һ��ͷ����Ϣ */
#define calculate_new_package_size(old_package,req_len) \
	((old_package->package_size) - (req_len) - sizeof(struct ring_package))

#define calculate_new_package_offset(old_package,req_len) \
	(ptr_to_cache_offset(old_package) + (req_len) + 2 * sizeof(struct ring_package))

/* ��ȡ������ͷ��ring package */
#define cache_head_ring_package(cache) ((struct ring_package *)((void *)cache + sizeof(*(cache))))

/* �����ھ�ring package */
#define add_to_prev_head(head, new, cache) do{\
	((struct ring_package *)cache_offset_to_ptr(head->next_head))->prev_head = ptr_to_cache_offset(new);\
	new->next_head             = head->next_head;\
	new->prev_head             = ptr_to_cache_offset(head);\
	head->next_head            = ptr_to_cache_offset(new);\
	}while(0)

/* ѭ���������Ƿ�Ϊ�� */
#define cache_is_empty(cache) ((cache_write_pos(cache) == cache_read_pos(cache)) &&\
								(cache_write_pos(cache)->valid_flag == INVALID_FLAG))

#define cache_is_full(cache) ((cache_write_pos(cache) == cache_read_pos(cache)) &&\
								(cache_write_pos(cache)->valid_flag))

#define get_new_package_from_slice(slice, slice_len) \
	(cache_offset_to_ptr((ptr_to_cache_offset(slice)+ (slice_len) + sizeof(struct ring_package))))

#define slice_new_package_offset(slice) \
	(ptr_to_cache_offset((slice))+ sizeof(struct ring_package))


#define get_cache_head_by_ring_pkt(pkt) \
	((pkt) - (pkt)->package_offset + sizeof(struct ring_package))

/**
	@brief ����һ���ڴ�����,��ʼ��Ϊring buff
	@return
		cache head �ɹ�   NULL ʧ��
*/
struct ring_buff_cache * ring_buff_head_init(void *cache, size_t length)
{
	struct ring_buff_cache *buff = (struct ring_buff_cache *)cache;
	struct ring_package *first_head = NULL;
	
	/* is init ? */
	if (buff->is_init)
		return buff;
	
	memset((void *)buff, 0, sizeof(struct ring_buff_cache));
	/* init buff cache head */
	buff->is_init           = 1;
	buff->cache_buff_length = length;
	buff->curr_read_pos     = sizeof(*buff);
	buff->curr_write_pos    = sizeof(*buff);

	/* ��ʼ����һ���ռ�buff */
	first_head = cache_head_ring_package(buff);

	first_head->next_head           = sizeof(struct ring_buff_cache);
	first_head->prev_head           = sizeof(struct ring_buff_cache);
	first_head->package_offset      = sizeof(struct ring_buff_cache) + sizeof(struct ring_package);
	first_head->package_size        = length - sizeof(struct ring_package) - sizeof(struct ring_buff_cache);
	
	return buff;
}



/*
	@brief �ӿ�buff�з���һ���µ�buff
	
*/
static void *slice_buff(struct ring_buff_cache *cache, struct ring_package *slice_head, size_t length)
{
	struct ring_package *new_head = get_new_package_from_slice(slice_head, length);
	/*
		����package head	
	*/
	new_head->valid_flag         = INVALID_FLAG;
	new_head->package_size       = calculate_new_package_size(slice_head, length);
	new_head->package_offset     = calculate_new_package_offset(slice_head, length);
	
	slice_head->package_size        = length;
	slice_head->package_offset      = slice_new_package_offset(slice_head);
	slice_head->valid_flag          = VALID_FLAG;

	/* add to prev head */
	add_to_prev_head(slice_head, new_head, cache);
	/* ������һ������buff �ռ� */
	cache->curr_write_pos = slice_head->next_head;

	return cache_offset_to_ptr(slice_head->package_offset);
	
}


/**
	@brief ����һ������ͷ
	@return
		�ɹ����ر���body  ʧ�ܷ��� NULL
*/
void *ring_buff_alloc(struct ring_buff_cache *cache, size_t length)
{
	struct ring_package *curr = NULL;
	int found = 0;

	/* length ����sizeof(long)����,��ĳЩǶ��ʽ�豸Ҫ��ָ����� */
	length = ALIGN(length,sizeof(long));
	
	/* IS FULL ? */
	if (cache_is_full(cache))
		return NULL;
	
	curr = cache_write_pos(cache);

	/* 
		���ҿ���packge head
		��Ϊ��buffֻ��һ���ҵ�
	*/	
	do
	{
		/* ��ǰbuff �Ƿ���Ч*/
		if (curr->valid_flag)
		{
			curr = (struct ring_package *)cache_offset_to_ptr(curr->next_head);
			continue;
		}
		/* good!! ����ȥ��Ƭ����package�� */
		if (curr->package_size == length)
		{
			curr->valid_flag = VALID_FLAG;
			/* ������һ����дbuff */
			cache->curr_write_pos = curr->next_head;

			return cache_offset_to_ptr(curr->package_offset);
		}
		
		/* ��ǰbuff ��С�Ƿ����� */
		if (!req_len_is_gt_package_len(curr, length))
		{
			found = 1;
			break;
		}

		curr = (struct ring_package *)cache_offset_to_ptr(curr->next_head);
	} while(curr != cache_read_pos(cache));

	/* �Ƿ�����ҵ��˿ɷ����buff */
	if (found)
		return slice_buff(cache, curr, length);

	return NULL;
}


/**
	@brief �ͷ�buff ���ϲ����ڿ���buff
	@return 
		��
*/
void ring_buff_free(struct ring_buff_cache *cache, void *buff)
{
	/* ���ͷŵ�ַ��Ӧ�ı���ͷ */
	struct ring_package *package;
	struct ring_package *prev = NULL, *next = NULL;
	int only_merge_prev = 0;
	
	package = buff_to_ring_package(buff);
	/* ����ͷ�Ƿ��ͷŹ� ? */
	if (!(package->valid_flag))
		return;
	
	package->valid_flag = INVALID_FLAG;
	
	/*
		�ͷŵ�ʱ��Ҫ��ȡ���ĵ�ǰ����ͷ,���Ƿ��п��б��Ŀ��Ժϲ�
	*/
	prev = get_prev_ring_package(package);
	next = get_next_ring_package(package);

	/* ���ǰ��ͺ������һ�����ڶ���ͷ����β�� ����ʱֻ��ϲ�һ����������� */
	if (next == cache_head_ring_package(cache))
		only_merge_prev = 1;

	if (package == cache_head_ring_package(cache))
		goto mereg_next;
	
	/* ǰ��buff ���Ժϲ� ? */
	if (!prev->valid_flag)
	{
		prev->next_head                           = package->next_head;
		get_next_ring_package(package)->prev_head = ptr_to_cache_offset(prev);
		prev->package_size                       += package->package_size + sizeof(*prev);

		/* У���ɶ���дcache λ�� */
		if (package == cache_write_pos(cache))
			cache->curr_write_pos = ptr_to_cache_offset(prev);

		if (package == cache_read_pos(cache))
			cache->curr_read_pos  = ptr_to_cache_offset(prev);
		
		package = prev;
	}

	if (only_merge_prev)
		return;
	
mereg_next:
	/* ����buff���Ժϲ� ? */
	if (!next->valid_flag)
	{
		package->next_head                     = next->next_head;
		get_next_ring_package(next)->prev_head = ptr_to_cache_offset(package);
		package->package_size                 += next->package_size + sizeof(*next);

		/* �����ʱcache��д�ɶ�ָ��ָ��next ��Ҫ���µ��� */
		if (next == cache_write_pos(cache))
			cache->curr_write_pos = ptr_to_cache_offset(package);

		if (next == cache_read_pos(cache))
			cache->curr_read_pos = ptr_to_cache_offset(package);
	}

	return;
}


/**
	@brief �ͷ�package ����
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
	@brief ��ȡ��ǰ�ɶ�ȡ����ͷ
	@return
		�пɶ����ķ���package  �޷��� NULL
*/
struct ring_package *ring_cache_read_package(struct ring_buff_cache *cache)
{
	struct ring_package *curr_read, *next_package;

	/* ��ǰ�ɶ�����λ��,��ǰһ��read package��ʱ������ */
	curr_read = cache_read_pos(cache);
	next_package = get_next_ring_package(curr_read);
		
	/* �޸��´οɶ�ȡ�ı���ͷ��λ�� */
	while (ptr_to_cache_offset(next_package) != cache->curr_write_pos)
	{
		/* �Ƿ�Ϊ�ɶ���־ */
		if (next_package->valid_flag == VALID_FLAG)
		{
			cache->curr_read_pos = ptr_to_cache_offset(next_package);
			goto out;
		}
			
		next_package = get_next_ring_package(next_package);
	}

	/* 
		oh,no ��ǰ�Ѿ��ǿɶ������һ��package head�� 
		��Ȼû���ҵ����ʵ���һ���ɶ�ring package
		��ô��һ���ɶ�ring package�϶�Ӧ����ָ��ǰ��дλ��
	*/
	cache->curr_read_pos = cache->curr_write_pos;
	
out:
	/* һ�㵱ǰָ��������Ч�� */
	if (curr_read->valid_flag == VALID_FLAG)
	{
		/* ��ǰpackage head ������ȡ, �޸���״̬Ϊbusy */
		curr_read->valid_flag = BUSY_FLAG;
		return curr_read;
	}
	
	return NULL;
}


/**
	@brief ����ʹ��,��ӡ���������е�package��Ϣ
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



