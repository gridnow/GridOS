/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net).
	All rights reserved.
*/

#include <fss.h>
#include <node.h>
#include <cache.h>

/**
	@brief Allocate the memory with give alignment

	@note The alignment must be 2^x
*/
static void * aligned_malloc(unsigned int size, unsigned int alignment)
{
	unsigned char * mem_ptr;
	unsigned char * tmp;

	/* No alignment ? minimum to sizeof(unsigned long) */
	if(!alignment) 
		alignment = sizeof(unsigned long);

	/* Allocate the required size memory + alignment so we can realign the data if necessary */
#ifdef _MSC_VER
	if ((tmp = (unsigned char *) malloc(size + alignment)) != NULL)
#else
	if ((tmp = (unsigned char *) km_valloc(size + alignment)) != NULL)
#endif
	{

		unsigned short * p;

		/* Align the tmp pointer */
		mem_ptr = (unsigned char *) ((unsigned long) (tmp + alignment - 1) & (~(unsigned long) (alignment - 1)));

		/* 
			Special case where malloc have already satisfied the alignment.
			We must add alignment to mem_ptr because we must store (mem_ptr - tmp) in *(mem_ptr-2)
		*/
        if (mem_ptr == tmp)
            mem_ptr += alignment;

		/* 
			(mem_ptr - tmp) is stored in *(mem_ptr-2) so we are able to retrieve
			the real malloc block allocated and free it in free.
		*/
		p =  (unsigned short*)(mem_ptr - 2);
		*p = (unsigned short )(mem_ptr - tmp);

// 		printf("memptr address is %p and size is %d\n", mem_ptr, size);
// 		printf("tmp address is %p\n",tmp);
// 		printf("offset is %d\n",*p);
		return ((void *)mem_ptr);
	}

	return NULL;
}

/**
	@brief aligned free

	@note The address must be allocated by aligned_malloc
*/
static void aligned_free(void *mem_ptr)
{
	unsigned short *p;
    unsigned char *ptr;

    if (mem_ptr == NULL)
        return;
	ptr = mem_ptr;
	p = (unsigned short*)(ptr - 2);
    
	/* Get the real address */
	ptr -= *p;
#ifdef _MSC_VER
	free(ptr);
#else
	km_vfree(ptr);
#endif	
}

static struct lv2_table * allocate_lv2(struct fss_mltt * mltt)
{
	struct lv2_table *p = aligned_malloc(sizeof(struct lv2_table), MLTT_PAGE_SIZE);
	if (!p) return p;	
	memset(p, 0, sizeof(struct lv2_table));

	/* Set the lv2 table base */
	mltt->lv2 = p;

	return p;
}

static struct lv1_table * allocate_lv1(struct fss_mltt * mltt, struct fss_mltt_pos * pos)
{
	struct lv1_table * p = aligned_malloc(sizeof(struct lv1_table), MLTT_PAGE_SIZE);
	if (!p) return p;	
	memset(p, 0, sizeof(struct lv1_table));

	/* Write back the pointer to top level */
	mltt->lv2->lv1[pos->lv2] = p;

	return p;
}

static void deallocate_lv2(struct lv2_table * table)
{
	aligned_free(table);
}

static void deallocate_lv1(struct lv1_table * table)
{
	aligned_free(table);
}

static void calc_mltt_pos(unsigned long block_id, struct fss_mltt_pos * pos)
{
	unsigned int lv_bits = 0;

	if (MLTT_ENTRY_COUNT == 1024)
		lv_bits = 10;
	else if (MLTT_ENTRY_COUNT == 2048)
		lv_bits = 11;
	else if (MLTT_ENTRY_COUNT == 4096)
		lv_bits = 12;
	else
	{
		fss_debug("MLTT Level error.\n");
		return;
	}

	pos->lv1 = block_id & ((1 << lv_bits) - 1);
	pos->lv2 = (block_id >> lv_bits) & ((1 << lv_bits) - 1);
}

/**
	@brief 搜索block_id指向的文件数据块

	搜索的过程是被锁住的，搜索到的dbd引用计数器加1.
*/
static struct dbd * search_mltt(struct fss_file * file, struct fss_mltt_pos * pos, unsigned long block_id)
{
	struct dbd * who = NULL;
	struct fss_mltt * mltt= &file->t.file.mltt;
	struct lv1_table * lv1;
	unsigned long lv1_entry;

	/* Enter critical section. */
	ke_spin_lock(&mltt->lock);
			
	/* But the table is created? */
	if (unlikely(!mltt->lv2))
	{
#if 0	/* small file */
		/* May be a small file which dose not have mltt */
		if (pos->lv1 < MLTT_STATIC_DBD_COUNT && pos->lv2 == 0)
		{
			who = &mltt->static_dbd[pos->lv1];
			goto end;
		}
		/* This is a big file access operation, so try to create the mltt */
		else
#endif
		{
			void * lv2_table;

			/* Tell kernel that the lock may be held for a long time due to page-fault in allocation. */
			ke_spin_lock_may_be_long(&mltt->lock);

			/* Allocate the table */
			lv2_table = allocate_lv2(mltt);
			if (!lv2_table)
				goto end;
		}
	}

	/* Get lower table */
	lv1 = (void*)((unsigned long)mltt->lv2->lv1[pos->lv2] & MLTT_TABLE_POINTER_MASK);
	if (!lv1)
	{
		/* Tell kernel that the lock may be held for a run long time due to page-fault in allocation. */
		ke_spin_lock_may_be_long(&mltt->lock);

		/* Allocate the table. */
		lv1 = allocate_lv1(mltt, pos);
		if (!lv1) goto end;
	}

	/* Get lv1 entry */
	lv1_entry = lv1->ptr[pos->lv1];
	if (!lv1_entry)
	{
		/* Allocate the dbd. */
		do
		{		
			lv1_entry = (unsigned long)fss_dbd_malloc(file);
			if (lv1_entry == NULL)
			{
				/* The lv1 table may be new or without entry, mask it not to be cleaned when we release MLTT lock */
				mltt->lv2->lv1[pos->lv2] = (struct lv1_table*)((unsigned long) mltt->lv2->lv1[pos->lv2] | MLTT_TABLE_USING);

				/* 将自身挂到等待队列,等待回收线程唤醒 */
				ke_spin_unlock(&mltt->lock);
				if (fss_grab_buffer() == false)
					goto err;
				ke_spin_lock(&mltt->lock);
				
				/* 可能已经被别的线程更新了,故重新取值 */
				lv1_entry = lv1->ptr[pos->lv1];

				mltt->lv2->lv1[pos->lv2] = (struct lv1_table*)((unsigned long) mltt->lv2->lv1[pos->lv2] & (~MLTT_TABLE_USING));
			}
		}while(lv1_entry == NULL);

		lv1->ptr[pos->lv1] = lv1_entry;

		/* Set the top table that we have one pointer */
		mltt->lv2->lv1[pos->lv2] = (void*)((unsigned long)mltt->lv2->lv1[pos->lv2] + 1);

		/* set offset */
		((struct dbd*)lv1_entry)->block_id = block_id;
	}
			
	/* lv1 is a dbd pointer. */
	who = (struct dbd *)(lv1_entry);

end:
	/* Cannot be deleted */
	ke_atomic_inc(&who->refcount);

	/* Leave critical section */
	ke_spin_unlock(&mltt->lock);

	/* The dbd may have no buffer */
	if (unlikely(who->buffer == NULL))
	{
		/* Check again with lock */
		ke_spin_lock(&who->lock);
		
		/* Other people may have already allocated one */
		if (who->buffer == NULL)
		{
			/* Allocate the Buffer for the DBD */
			do
			{
				/* 如果分配不成功，则抢占 */
				if ((who->buffer = fss_db_malloc()) == NULL)
				{
					ke_spin_lock_may_be_long(&who->lock);
					if (fss_grab_buffer() == false)
					{
						ke_spin_unlock(&who->lock);
						goto err;
					}
				}				
			} while (who->buffer == NULL);

			/* No buffer means not in LRU, add it by the way */
			fss_dbd_lru_add(who);
		}

		ke_spin_unlock(&who->lock);
	}		

	/* Increase access count and Update LRU list */
	fss_dbd_lru_update(who);
	return who;
err:
	return NULL;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

/**
	@brief Locate the mltt entry and clean it if nobody using it
*/
bool fss_mltt_entry_clean_nolock(struct dbd * where)
{
	bool r = false;
	struct fss_mltt * mltt= &where->who->t.file.mltt;
	struct lv1_table * lv1;
	unsigned long lv1_entry;
	struct fss_mltt_pos pos;

	/* 静态DBD不断开链接 */
	if (where->flags & DB_STAIC)
		return true;

	/* To the mltt position */
	calc_mltt_pos(where->block_id, &pos);
			
	/* Get lower table */
	lv1 = (void*)((unsigned long)mltt->lv2->lv1[pos.lv2] & MLTT_TABLE_POINTER_MASK);

	/* Get lv1 entry */
	lv1_entry = lv1->ptr[pos.lv1];

	/* Null it */
	lv1->ptr[pos.lv1] = (unsigned long)NULL;
	r = true;

	/* The entry in lv1 is cleaned, the table pointer count should be decreased too */
	mltt->lv2->lv1[pos.lv2] = (void*)((unsigned long)mltt->lv2->lv1[pos.lv2] - 1);
	if (((unsigned long)mltt->lv2->lv1[pos.lv2] & MLTT_TABLE_COUNTER_MASK) == 0)
	{
		/* The lv1 table may be new or without entry, mask it not to be cleaned when we release MLTT lock */
		if ((unsigned long)mltt->lv2->lv1[pos.lv2] & MLTT_TABLE_USING)
		{
			fss_debug("The sub-table( LV1 %x) is empty but is in USING.\n", lv1);
		}
		else
		{
			fss_debug("The sub-table( LV1 %x) is empty and can be recalled.\n", lv1);
			mltt->lv2->lv1[pos.lv2] = NULL;
			deallocate_lv1(lv1);
		}
	}
	
	return r;
}


/**
	@brief 通过访问块ID获取对应缓存描述符
*/
struct dbd *fss_dbd_get(struct fss_file * who, unsigned long block_id)
{	
	struct fss_mltt_pos pos;
	struct dbd * dbd;

	calc_mltt_pos(block_id, &pos);	
	dbd = search_mltt(who, &pos, block_id);
	
	//printf("Getting dbd by offset %llx ->(%d.%d.%d).\n", offset, pos.lv2, pos.lv1, pos.lv0);
	return dbd;
}

/**
	@brief Aligned malloc
*/
void * fss_aligned_malloc(unsigned int size, unsigned int alignment)
{
	return aligned_malloc(size, alignment);
}

/**
	@brief Aligned mem free
*/
void fss_aligned_free(void * p)
{
	aligned_free(p);
}

/**
	@brief Release the holding of a dbd
*/
void fss_dbd_put(struct dbd * which)
{
	ke_atomic_dec(&which->refcount);
}

/**
	@brief Init the mltt data structure for a file
*/
void fss_mltt_init(struct fss_file * who)
{
	struct fss_mltt *mltt = &who->t.file.mltt;
	if (who->type == FSS_FILE_TYPE_NORMAL)
		ke_spin_init(&mltt->lock);
}
