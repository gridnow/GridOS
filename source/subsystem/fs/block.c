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
/* 内部数据                                                             */
/************************************************************************/
static struct fss_cache static_cache_desc;

/************************************************************************/
/* 内部逻辑                                                             */
/************************************************************************/
static struct fss_cache *cache_desc()
{
	return &static_cache_desc;
}

/************************************************************************/
/*外部接口                                                              */
/************************************************************************/


/**
	@brief 分配新的DB空间
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
	@brief 分配新的DBD空间
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
	@brief 回收DBD空间
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
	@brief 更新LRU排序，降低淘汰率
*/
void fss_dbd_lru_update(struct dbd * dbd)
{
	struct list_head * next;

	/* 增加其访问频率 */
	ke_atomic_inc(&dbd->access_counter);
	
	if ((ke_atomic_read(&dbd->access_counter) & FSS_DBD_LEVEL_EXCHANGE_MASK) == 0)
	{	
		ke_spin_lock(&cache_desc()->sort_lock);

		/* 如果只有一个节点，无需交换 */
		if (unlikely(list_is_singular(&cache_desc()->sort_list)))
			goto no_update;

		/* 与下一个DBD交换位置 */
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
		调用者保证which是被锁住的
*/
void fss_dbd_lru_add(struct dbd * which)
{
	/* 插入尾部是最不会被淘汰的，新来的DBD的淘汰率最低 */
	ke_spin_lock(&cache_desc()->sort_lock);
	list_add_tail(&which->list, &cache_desc()->sort_list);
	ke_spin_unlock(&cache_desc()->sort_lock);
}

/**
	@brief DB cache 反初始化
*/
void fss_db_deinit(void)
{
	//TODO
	return;
}

/**
	@brief 线程自我主动阻塞

	@note
		如果当前线程异常，那么会返回失败，否则永远不会。
*/
bool fss_grab_buffer(void)
{	
	bool ret = true;
	
	/* We are the one to wait the result */
	ke_atomic_inc(&cache_desc()->recaller->desired);

	/* Trigger recaller */
	ke_event_set(cache_desc()->recaller->start_recall);

	/* 等待完成，如果太多人都在等待，并且回收了很多的DBD，那么大家就会疯抢DBD，应该有个优先级控制 */
	//TODO:增加优先级控制，避免疯抢！
	if (ke_event_wait(cache_desc()->recaller->end_recall, KE_SYNC_WAIT_INFINITE) != KE_WAIT_OK)
		ret = false;

	return ret;
}

void fss_dbd_add_to_dirty(struct dbd * which)
{
	/* 空的，没有插入到dirty list */
	if (list_empty(&which->dirty_list))
	{
		/* 锁住再查，可能被其他线程插入 */
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
	@brief DB cache初始化
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
	cache_desc()->db_cache = km_alloc_virtual(cache_desc()->cache_size, KM_PROT_READ | KM_PROT_WRITE);
	printf("fss_db_cache = %x", cache_desc()->db_cache);
#endif
	if (cache_desc()->db_cache == NULL)
		goto err1;
	
	/* 分配DB栈 */
#ifdef _MSC_VER
	cache_desc()->db_stack = (unsigned long *)malloc(cache_desc()->db_count * sizeof(unsigned long));
#else
	cache_desc()->db_stack = (unsigned long *)km_valloc(cache_desc()->db_count * sizeof(unsigned long));
	printf(", fss_db_stack = %x.\n", cache_desc()->db_stack);
#endif
	if (cache_desc()->db_stack == NULL)	
		goto err2;
	
	/* DB节点入栈 */	
	for (i = 0; i < cache_desc()->db_count; i++)
		cache_desc()->db_stack[i] = (unsigned long)cache_desc()->db_cache + (i * FSS_CACHE_DB_SIZE);
	cache_desc()->db_stack_top = cache_desc()->db_count - 1;

	/* 分配DBD */
#ifdef _MSC_VER
	cache_desc()->dbd_cache = (struct dbd*)malloc((cache_desc()->db_count) * sizeof(struct dbd));
#else
	cache_desc()->dbd_cache = (struct dbd*)km_valloc((cache_desc()->db_count) * sizeof(struct dbd));
#endif
	if (cache_desc()->dbd_cache == NULL)
		goto err3;

	/* DBD节点入栈 */				
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
