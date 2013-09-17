/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内存管理
 */

#ifndef KM_WALK_H
#define KM_WALK_H

#include <debug.h>
#include <spinlock.h>

#include <arch/page.h>

#define KM_PAGE_PHY_FROM_ENTRY(entry, level) ((entry) & PAGE_MASK)				//TODO: to support non page_mask size

/* Default table entry count is 1K for 4kb page */
#ifndef ARCH_KM_LV1_COUNT
#define ARCH_KM_LV1_COUNT	1024
#endif
#ifndef ARCH_KM_LV2_COUNT
#define ARCH_KM_LV2_COUNT	1024
#endif
#ifndef KM_WALK_MAX_LEVEL
#define KM_WALK_MAX_LEVEL	2
#endif

#define KM_WALK_INIT(MEM, WALK) do { \
	(WALK)->miss_action = km_walk_miss; \
	(WALK)->mem = (MEM);	\
} while(0)

struct km
{
	spinlock_t lock;
	void *translation_table;

	/* 地址操作栈 */
#define KM_MAX_ADDRESS_STACK 32
	int address_idx;
	unsigned long address_array[KM_MAX_ADDRESS_STACK];
	struct list_head wait_queue[KM_MAX_ADDRESS_STACK];
};

struct km_walk_ctx
{
#define level_id	hirarch_id[0]

	struct km		*mem;
	int				hirarch_id[KM_WALK_MAX_LEVEL + 1];
	unsigned long	*table_base[KM_WALK_MAX_LEVEL + 1];
	unsigned long	current_virtual_address;
	
	union
	{
		void *(*miss_action)(struct km_walk_ctx *ctx);
		void  (*meet_manage_action)(struct km_walk_ctx *ctx, void *subtable);
	};
};

bool km_walk_to(struct km_walk_ctx *ctx, unsigned long va);
void *km_walk_miss(struct km_walk_ctx *ctx);
void *km_walk_alloc_table(struct km_walk_ctx *ctx);
void *km_create_sub_table(struct km_walk_ctx *ctx, void *table, int sub_id);
void km_walk_init_for_kernel(struct km *mem);
bool km_walk_init(struct km *mem);

/*
	General version of walk-related functions.
*/
#ifndef ARCH_HAS_KM_GET_VID
static inline unsigned long km_get_vid(unsigned long level, unsigned long va)
{
	if (sizeof(unsigned long) == 4)
	{
		switch (level)
		{
			case 2:
				va >>= 22;
				va &= (ARCH_KM_LV2_COUNT - 1);
				break;
				
			case 1:
				va >>= 12;
				va &= (ARCH_KM_LV1_COUNT - 1);
				break;
				
			default:
				va = -1;
		}
	}
	else
	{
		//TODO level3?
		va = -1;
	}
	
	return va;
}
#endif

#ifndef ARCH_HAS_PTE_T
typedef unsigned long pte_t;
static inline void km_pte_write(struct km_walk_ctx * ctx, pte_t what)
{
	void *entry  = &(ctx->table_base[1][ctx->hirarch_id[1]]);
	pte_t *p = entry;
	
	//	printk("kmm_pte_write: ctx->table_base[0] = %p, id %d, entry %p(%p), what %p.\n",
	//	   		ctx->table_base[1],
	//	   		ctx->hirarch_id[1],
	//	   		entry, *(unsigned long*)entry,
	//	   		what);
	
	/* Sanity check */
 	if (*p)
	{
		printk("ERROR:PTE entry is not empty.\n");
		return;
	}
	*p = what;
}

#endif

/**
	@brief Adjust to next pte entry
 */
static inline bool km_pte_next(struct km_walk_ctx * ctx)
{
	ctx->current_virtual_address += PAGE_SIZE;
	if (likely(ctx->hirarch_id[1] + 1 != ARCH_KM_LV1_COUNT))
		ctx->hirarch_id[1]++;
	else
		return km_walk_to(ctx, ctx->current_virtual_address);

	return true;
}

static inline pte_t km_pte_read(struct km_walk_ctx *ctx)
{
	pte_t *entry  = &(ctx->table_base[1][ctx->hirarch_id[1]]);
	return *entry;
}
	
static inline bool km_pte_write_and_next(struct km_walk_ctx *ctx, pte_t what)
{
	km_pte_write(ctx, what);
	return km_pte_next(ctx);
}

static inline bool km_pte_read_and_next(struct km_walk_ctx *ctx, pte_t *pte)
{
	*pte = km_pte_read(ctx);
	return km_pte_next(ctx);
}

#endif