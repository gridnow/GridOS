/**
 *   The File Subsystem
 *   Wuxin
 */

#ifndef _MSC_VER
#include <types.h>
#endif
#include <stdio.h>

#include <list.h>
#include <ddk/vfs.h>

#include "cache.h"
/************************************************************************/
/* �ڲ�����                                                             */
/************************************************************************/
static struct fss_cache static_cache_desc;

/************************************************************************/
/* �ڲ��߼�                                                             */
/************************************************************************/
static struct fss_cache *cache_desc()
{
	return &static_cache_desc;
}

/************************************************************************/
/*�ⲿ�ӿ�                                                              */
/************************************************************************/


/**
	@brief �����µ�DB�ռ�
*/
void* fss_db_malloc(void)
{
	void* ret = NULL;
	
	ke_spin_lock(&cache_desc()->db_stack_lock);
	if (cache_desc()->db_stack_top >= 0)
	{
		ret = (void*)cache_desc()->db_stack[cache_desc()->db_stack_top];
		cache_desc()->db_stack_top--;
	}
	ke_spin_unlock(&cache_desc()->db_stack_lock);

	/* Really need to set to zero ? */
#if 0
	if (ret != NULL)
	{
		memset(ret, 0, FSS_CACHE_DB_SIZE);
	}

#endif
	return ret;
} 

/**
	@brief �����µ�DBD�ռ�
*/
struct dbd* fss_dbd_malloc(struct fss_file * who)
{
	struct dbd * ret = NULL;	

	ke_spin_lock(&cache_desc()->dbd_stack_lock);
	if (cache_desc()->dbd_stack_top != NULL)
	{
		ret = cache_desc()->dbd_stack_top;
		cache_desc()->dbd_stack_top = (struct dbd *)cache_desc()->dbd_stack_top->list.prev;		
	}
	ke_spin_unlock(&cache_desc()->dbd_stack_lock);
	
	if (ret != NULL)
	{
		memset(ret, 0, sizeof(struct dbd));	
		ke_spin_init(&ret->lock);
		
		/* The dbd may be deleted before entering the LRU lit, so it's empty */
		INIT_LIST_HEAD(&ret->list);

		/* Dirty list is empty, we will insert it to global dirty list */
		INIT_LIST_HEAD(&ret->dirty_list);

		/* No map */
		INIT_LIST_HEAD(&ret->map_list);
		ke_spin_init(&ret->map_lock);
		ret->who = who;
	}

	return ret;
}

/**
	@brief ����DBD�ռ�
*/
void fss_dbd_free(struct dbd * dbd)
{		
	if (dbd->buffer != NULL)
	{
		ke_spin_lock(&cache_desc()->db_stack_lock);
		cache_desc()->db_stack_top++;	
		cache_desc()->db_stack[cache_desc()->db_stack_top] = (unsigned long)dbd->buffer;
		ke_spin_unlock(&cache_desc()->db_stack_lock);	

		dbd->buffer = NULL;
	}

	ke_spin_lock(&cache_desc()->dbd_stack_lock);
	dbd->list.prev = (struct list_head *)cache_desc()->dbd_stack_top;
	cache_desc()->dbd_stack_top = dbd;
	dbd->who = NULL;
	ke_spin_unlock(&cache_desc()->dbd_stack_lock);	

	return;
}

/**
	@brief ����LRU���򣬽�����̭��
*/
void fss_dbd_lru_update(struct dbd * dbd)
{
	struct list_head * next;

	/* ���������Ƶ�� */
	ke_atomic_inc(&dbd->access_counter);
	
	if ((ke_atomic_read(&dbd->access_counter) & FSS_DBD_LEVEL_EXCHANGE_MASK) == 0)
	{	
		ke_spin_lock(&cache_desc()->sort_lock);

		/* ���ֻ��һ���ڵ㣬���轻�� */
		if (unlikely(list_is_singular(&cache_desc()->sort_list)))
			goto no_update;

		/* ����һ��DBD����λ�� */
		next = dbd->list.next;
		list_del(&dbd->list);
		list_add(&dbd->list, next);
no_update:
		ke_spin_unlock(&cache_desc()->sort_lock);
	}
	
	return;
}

/**
	@note
		�����߱�֤which�Ǳ���ס��
*/
void fss_dbd_lru_add(struct dbd * which)
{
	/* ����β������ᱻ��̭�ģ�������DBD����̭����� */
	ke_spin_lock(&cache_desc()->sort_lock);
	list_add_tail(&which->list, &cache_desc()->sort_list);
	ke_spin_unlock(&cache_desc()->sort_lock);
}

/**
	@brief DB cache ����ʼ��
*/
void fss_db_deinit(void)
{
	//TODO
	return;
}

/**
	@brief �߳�������������

	@note
		�����ǰ�߳��쳣����ô�᷵��ʧ�ܣ�������Զ���ᡣ
*/
bool fss_grab_buffer(void)
{	
	bool ret = true;
	
	/* We are the one to wait the result */
	ke_atomic_inc(&cache_desc()->recaller->desired);

	/* Trigger recaller */
	ke_event_set(cache_desc()->recaller->start_recall);

	/* �ȴ���ɣ����̫���˶��ڵȴ������һ����˺ܶ��DBD����ô��Ҿͻ����DBD��Ӧ���и����ȼ����� */
	//TODO:�������ȼ����ƣ����������
	if (ke_event_wait(cache_desc()->recaller->end_recall, KE_SYNC_WAIT_INFINITE) != KE_WAIT_OK)
		ret = false;

	return ret;
}

void fss_dbd_add_to_dirty(struct dbd * which)
{
	/* �յģ�û�в��뵽dirty list */
	if (list_empty(&which->dirty_list))
	{
		/* ��ס�ٲ飬���ܱ������̲߳��� */
		ke_spin_lock(&which->lock);
		if (list_empty(&which->dirty_list))
		{
			ke_spin_lock(&cache_desc()->dirty_list_lock);
			list_add(&which->dirty_list, &cache_desc()->dirty_list);
			ke_spin_unlock(&cache_desc()->dirty_list_lock);
		}
		ke_spin_unlock(&which->lock);
	}
}

/**
	@brief DB cache��ʼ��
*/
bool fss_db_init(void)
{	
	int i = 0;
	struct dbd *dbd_prev = NULL;

	cache_desc()->cache_size = FSS_CACHE_SIZE;
	cache_desc()->db_count = cache_desc()->cache_size / FSS_CACHE_DB_SIZE;	 

	/* The db virtual memory */
#ifdef _MSC_VER
	cache_desc()->db_cache = (char*)malloc(cache_desc()->cache_size);
#else
	cache_desc()->db_cache = km_alloc_virtual(cache_desc()->cache_size, KM_PROT_READ | KM_PROT_WRITE, NULL);
	printf("fss_db_cache = %x", (unsigned int)cache_desc()->db_cache);
#endif
	if (cache_desc()->db_cache == NULL)
		goto err1;
	
	/* ����DBջ */
#ifdef _MSC_VER
	cache_desc()->db_stack = (unsigned long *)malloc(cache_desc()->db_count * sizeof(unsigned long));
#else
	cache_desc()->db_stack = (unsigned long *)km_valloc(cache_desc()->db_count * sizeof(unsigned long));
	printf(", fss_db_stack = %x.\n", (unsigned int)cache_desc()->db_stack);
#endif
	if (cache_desc()->db_stack == NULL)	
		goto err2;
	
	/* DB�ڵ���ջ */	
	for (i = 0; i < cache_desc()->db_count; i++)
		cache_desc()->db_stack[i] = (unsigned long)cache_desc()->db_cache + (i * FSS_CACHE_DB_SIZE);
	cache_desc()->db_stack_top = cache_desc()->db_count - 1;

	/* ����DBD */
#ifdef _MSC_VER
	cache_desc()->dbd_cache = (struct dbd*)malloc((cache_desc()->db_count) * sizeof(struct dbd));
#else
	cache_desc()->dbd_cache = (struct dbd*)km_valloc((cache_desc()->db_count) * sizeof(struct dbd));
#endif
	if (cache_desc()->dbd_cache == NULL)
		goto err3;

	/* DBD�ڵ���ջ */				
	dbd_prev = NULL;
	for (i = 0; i < (cache_desc()->db_count); i++)
	{
		cache_desc()->dbd_stack_top = &(cache_desc()->dbd_cache[i]);
		cache_desc()->dbd_stack_top->buffer = NULL;
		cache_desc()->dbd_stack_top->list.prev = (void*)dbd_prev;
		dbd_prev = cache_desc()->dbd_stack_top;
	}
	ke_spin_init(&cache_desc()->db_stack_lock);
	ke_spin_init(&cache_desc()->dbd_stack_lock);
	ke_spin_init(&cache_desc()->dirty_list_lock);

	INIT_LIST_HEAD(&cache_desc()->sort_list);
	INIT_LIST_HEAD(&cache_desc()->dirty_list);
	ke_spin_init(&cache_desc()->sort_lock);

	//TODO:
	//Write back thread
	return true;

err3:
	km_vfree(cache_desc()->db_stack);
err2:
	km_dealloc_virtual(cache_desc()->db_cache);
	cache_desc()->db_count = 0;		
err1:
	return false;
}
