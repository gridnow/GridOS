/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   –Èƒ‚µÿ÷∑π‹¿Ì∆˜
 */
#include <debug.h>
#include <string.h>

#include <memory.h>
#include <walk.h>

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

void km_walk_init_for_kernel(struct km *mem)
{
	spin_lock_init(&mem->lock);
	km_arch_init_for_kernel(mem);
}

bool km_walk_init(struct km *mem)
{
	struct km_walk_ctx ctx;
	
	spin_lock_init(&mem->lock);
	
	return true;
}
