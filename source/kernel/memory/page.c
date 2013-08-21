/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   –Èƒ‚µÿ÷∑π‹¿Ì∆˜
 */

#include <types.h>
#include <page.h>
#include <walk.h>
#include <memory.h>

/**
	@brief Deallocate the pages in the section range
*/
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
	
end:
	return true;
	
err:
	TODO("Cleanup PTE for failed mapping");
	return false;
}

/**
	@brief Create a page on VM 
*/
bool km_page_create(struct km *mem_dst, unsigned long address, page_prot_t prot)
{
	pte_t entry;
	unsigned long page = 0;
	struct km_walk_ctx dst_ctx;

	KM_WALK_INIT(mem_dst, &dst_ctx);
	
	if (unlikely(km_walk_to(&dst_ctx, address) == false))
		goto err;
	if (km_pte_read(&dst_ctx))
		goto err;
	page = km_page_alloc();
	if (!page)
		goto err;
	entry = page | km_arch_get_flags(prot);	
	km_pte_write(&dst_ctx, entry);

	return true;

err:
	if (page)
		km_page_dealloc(page);
	return false;
}
