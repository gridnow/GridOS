/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   虚拟地址管理器
 */

#include <types.h>
#include <page.h>
#include <walk.h>
#include <memory.h>

#include "cl_string.h"

static void delete_page(struct km_walk_ctx *ctx, pte_t pte)
{	
	unsigned long phy = KE_PAGE_PHY_FROM_PTE(pte);		
	if (KE_PAGE_INVALID_PTE(pte) == 0)
	{
		if (KE_PAGE_IS_SHARED_FROM(pte) == 0)
		{
			//printk("Delete page for virtual %x, pte %x, level_id %x.\n", ctx->virtual_address, pte, ctx->level_id);
			km_page_dealloc(phy);		
			//printk("deallcated phy = %p\n", phy);
		}
		km_pte_clean(ctx); 
	}

	/* If the manager table is empty, delete the table too */
	//TODO
}

bool km_page_map_range(struct km *mem_dst, unsigned long start_va, unsigned long size, unsigned long physical_pfn, page_prot_t prot)
{
	long i;
	struct km_walk_ctx dst_ctx;
	unsigned long physical_address = physical_pfn << PAGE_SHIFT;
 
	KM_WALK_INIT(mem_dst, &dst_ctx);
	
	if (unlikely(km_walk_to(&dst_ctx, start_va) == false))
		goto err;
	for (i = size; i > 0; i -= PAGE_SIZE, physical_address += PAGE_SIZE)
	{
		if (unlikely(km_pte_write_and_next(&dst_ctx, physical_address | km_arch_get_flags(prot) | PAGE_FLAG_FROM_OTHER/*PHY is from other*/) == false))
			goto err;
	} 
	
	/* 不需要flush tlb，因为新映射的page不在tlb中 */
	return true;
	
err:
	TODO("Cleanup PTE for failed mapping");
	return false;
}

bool km_page_create(struct km *mem_dst, unsigned long address, page_prot_t prot)
{
	pte_t entry;
	unsigned long page = 0;
	struct km_walk_ctx dst_ctx;

	KM_WALK_INIT(mem_dst, &dst_ctx);
	
	if (unlikely(km_walk_to(&dst_ctx, address) == false))
		goto err;

	/* Create 模式应该看不到任何内容，否则可以理解被别人刚刚处理成功 */
	if (km_pte_read(&dst_ctx))
		goto err;
	page = km_page_alloc();
	if (!page)
		goto err;
	entry = page | km_arch_get_flags(prot);	
	km_pte_write(&dst_ctx, entry);
	
	/* 不需要flush tlb，因为新的page在tlb中是不存在的 */
	return true;

err:
	if (page)
		km_page_dealloc(page);
	return false;
}

bool km_page_create_cow(struct km *mem_dst, unsigned long address)
{
	pte_t entry;
	unsigned long page = 0;
	struct km_walk_ctx dst_ctx;
	
	KM_WALK_INIT(mem_dst, &dst_ctx);
	
	if (unlikely(km_walk_to(&dst_ctx, address) == false))
		goto err;
	
	/* 必须有内容，否则就不应该Cow */
	if (!km_pte_read(&dst_ctx))
		goto err;
	page = km_page_alloc();
	if (!page)
		goto err;
	
	//TODO: Non identical map
	if (page >= CONFIG_HAL_KERNEL_MEM_LEN)
		goto err;
	memcpy((void*)HAL_GET_BASIC_KADDRESS(page), (void*)KM_PAGE_ROUND_ALIGN(address), PAGE_SIZE);
	
	entry = page | km_arch_get_flags(KM_PROT_READ | KM_PROT_WRITE);
	km_pte_write_force(&dst_ctx, entry);

	/* Must flush, because we are changing the entry */
	km_arch_flush_page(address);

	return true;
	
err:
	if (page)
		km_page_dealloc(page);
	return false;
}

int km_page_share(struct km *dst, unsigned long dst_addr, struct km *src, unsigned long src_addr, page_prot_t prot)
{
	unsigned long i, src_end = src_addr + PAGE_SIZE;
	int r = KM_PAGE_SHARE_RESULT_ERR;
	unsigned long old;		
	
	struct km_walk_ctx dst_ctx, src_ctx;
	
	/* Walk to it */
	KM_WALK_INIT(dst, &dst_ctx);
	KM_WALK_INIT(src, &src_ctx);
	if (unlikely(km_walk_to(&dst_ctx, dst_addr) == false))
		goto end;
	if (unlikely(km_walk_to(&src_ctx, src_addr) == false))
		goto end;
	old = km_pte_read(&dst_ctx);
	if (unlikely(old))
	{
	//	r = KM_PAGE_SHARE_RESULT_OK;
		goto end;		
	}

 	/* Write entry one by one */
 	for (i = src_addr; i < src_end; i += PAGE_SIZE)
 	{
		pte_t pte;

		/* Read src one by one */
		if (unlikely(km_pte_read_and_next(&src_ctx, &pte) == false))
		{
			goto end;
		}

		/* Not a valid pte */
		if (KE_PAGE_INVALID_PTE(pte))
		{
//			printk("invalid src pte %x, i = %d.\n", pte, i);
			r = KM_PAGE_SHARE_RESULT_SRC_INVALID;
			goto end;
		}

		pte &= ~PAGE_FLAG_MASK;
		pte |= km_arch_get_flags(prot) | PAGE_FLAG_FROM_OTHER;
		if (unlikely(km_pte_write_and_next(&dst_ctx, pte) == false))
		{
			goto end;
		}
 	}
 	r = KM_PAGE_SHARE_RESULT_OK;
	
	/* 不需要flush tlb，因为share的page在tlb以前是不存在的。*/
	
end:
	return r;
}

void km_page_share_kernel(struct km* mem_dst, unsigned long virtual_address)
{
	/* Call arch to copy it */
	km_arch_copy_kernel(mem_dst, virtual_address);
	
	/* 不需要flush tlb，因为share的page在tlb以前是不存在的。*/
}

void km_page_dealloc_range(struct km *mem_dst, unsigned long start, unsigned long size)
{
	struct km_walk_ctx dst_ctx;

	KM_WALK_INIT(mem_dst, &dst_ctx);	
	km_walk_loop(&dst_ctx, start, size, delete_page);

	/* Flush the TLB */
	km_arch_flush_pages(start, size);
}