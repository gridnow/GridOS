/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   虚拟地址管理器
 */
#include <debug.h>
#include <string.h>

#include <memory.h>
#include <walk.h>
#include <process.h>

#include <sync.h>

void *km_get_sub_table(unsigned long *table, int sub_id)
{
	unsigned long sub_table_phy;
	
	sub_table_phy = KM_PAGE_PHY_FROM_ENTRY(table[sub_id], sub_id);
	if (sub_table_phy == NULL)
		return NULL;
	//TODO: to support non-identical mapping tables
	return (void*)HAL_GET_BASIC_KADDRESS(sub_table_phy);
}

/**
	@brief Walk to a given virtual address
 
	@note
		Memory ctx should be "got"
*/
bool km_walk_to(struct km_walk_ctx *ctx, unsigned long va)
{
	int i;
	bool r = false;
	void *table;
	
	ctx->level_id					= i = KM_WALK_MAX_LEVEL;
	ctx->current_virtual_address	= va;
	
	table			= ctx->mem->translation_table;

	do
	{
		int sub_id;
		
		if (unlikely(table == NULL))
		{
			if (NULL == ctx->miss_action ||
				NULL == (table = ctx->miss_action(ctx)))
				goto end;
		}
		
		sub_id = km_get_vid(i, ctx->current_virtual_address);
		ctx->hirarch_id[i]		= sub_id;
		ctx->table_base[i]		= table;
		ctx->level_id			= i;
		
		if (--i == 0)
			break;
		
		table = km_get_sub_table(table, sub_id);
	} while (1);
	r = true;
	
end:	
	return r;
}

void *km_walk_alloc_table(struct km_walk_ctx *ctx)
{
	void *p;
	
	//TODO:support non-identical mapping
	p = km_page_alloc_kerneled(1);
	if (!p) goto end;
	memset(p, 0, PAGE_SIZE);
	
end:
	return p;
}

void *km_walk_miss(struct km_walk_ctx *ctx)
{
	void * table;
	
	if (unlikely(ctx->mem->translation_table == NULL))
	{
		table = km_walk_alloc_table(ctx);
		if (!table) goto end;
		ctx->mem->translation_table = table;
	}
	else
	{
		int sub_id = ctx->hirarch_id[ctx->level_id];
		table = ctx->table_base[ctx->level_id];
		table = km_create_sub_table(ctx, table, sub_id);/* To create sub table */
	}
	
end:
	return table;
}

void *km_create_sub_table(struct km_walk_ctx *ctx, void *table, int sub_id)
{
	void *sub_table;
	unsigned long sub_table_phy;
	
	sub_table = km_walk_alloc_table(ctx);
	if (unlikely(sub_table == NULL))
		return NULL;
	
	//TODO: to support non-identical mapping tables
	sub_table_phy = HAL_GET_BASIC_PHYADDRESS(sub_table);
	km_write_sub_table(table, sub_id, sub_table_phy);
	return sub_table;
}

/**
	@brief 准备对address进行操作
*/
struct km *kp_get_mem(struct ko_process *who)
{
	struct km *mem = &who->mem_ctx;

#if 0 /* 细粒度的还没有用，并且细粒度在page walk的时候还需要锁来处理translation table 互斥问题 */
	int met = 0;
	int i, null_id = -1;
	struct thread_wait wait;

claim_again:
	spin_lock(&mem->lock);

	/* Address can be merged? */
	for (i = 0; i < KM_MAX_ADDRESS_STACK; i++)
	{
		if (KM_PAGE_ROUND_ALIGN(address) == mem->address_array[i])
		{
			met = 1;
			goto wait_this_address;
		}
		if (null_id == -1 && mem->address_array[i] == -1UL)
			null_id = i;
	}

	/* No empty slot? wait a random index to finish */
	if (null_id == -1)
		goto wait_this_address;

	/* Has an empty slot, goto into it and do what you want */
	else
	{
		mem->address_array[null_id] = KM_PAGE_ROUND_ALIGN(address);
		*key = null_id;
	}
	spin_unlock(&mem->lock);

	return mem;

wait_this_address:
	/* Goto wait list */
	if (met == 1)
		KT_PREPARE_WAIT(mem->wait_queue[i/*wait the mached to finish*/], &wait);
	else
		KT_PREPARE_WAIT(mem->wait_queue[0/*满了，随机选择是不是更好？*/], &wait);
	spin_unlock(&mem->lock);

	if (met == 1)
	{
		if (KT_WAIT(KM_PAGE_ROUND_ALIGN(address) != mem->address_array[i]) == KE_WAIT_ABANDONED)
			goto abandoned;
	}
	else
	{
		/* 由于没有slot而休眠，那么唤醒后继续再去找slot */
		if (KT_WAIT(-1UL == mem->address_array[0]) == KE_WAIT_ABANDONED)
			goto abandoned;
		
		goto claim_again;
	}

	return NULL; 

abandoned:
	/* The thread is forced not to wait, AND let the thread out, it will be killed at exception return */
	spin_lock(&mem->lock);
	list_del(&wait.task_list);
	spin_unlock(&mem->lock);

	return NULL;
#else
	spin_lock(&mem->lock);
	return mem;
#endif
}

void kp_put_mem(struct km *mem)
{
#if 0/* See get */
	struct thread_wait *wait, *n;
	int count = 0;
	unsigned long address ;

	spin_lock(&mem->lock);

	address = mem->address_array[key];
	mem->address_array[key] = -1UL;

	/* Has waiter on this merged address? */
	list_for_each_entry_safe(wait, n, &mem->wait_queue[key], task_list)
	{
		/* We have to use "init" version, because the thread my be abandoned and release the list by itself */
		list_del_init(&wait->task_list);
		kt_wakeup(wait->who);
		count ++;
	}

	//printk("Wakeup %d waiter for address %x, key is %d.\n", count, address, key);

	spin_unlock(&mem->lock);
#else
	spin_unlock(&mem->lock);
#endif
}

void km_walk_init_for_kernel(struct km *mem)
{
	spin_lock_init(&mem->lock);
	km_arch_init_for_kernel(mem);
}

bool km_walk_init(struct km *mem)
{
	int i;
	struct km_walk_ctx ctx;
	
	spin_lock_init(&mem->lock);
	for (i = 0; i < KM_MAX_ADDRESS_STACK; i++)
	{
		INIT_LIST_HEAD(&mem->wait_queue[i]);
		mem->address_array[i] = -1UL;
	}

	/* Walk to NULL will create the Very First table */
	KM_WALK_INIT(mem, &ctx);
	if (unlikely(km_walk_to(&ctx, 0) == false))
		goto err;
	return true;

err:
	return false;
}
