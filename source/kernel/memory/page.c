/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   �����ַ������
 */

#include <types.h>
#include <page.h>
#include <walk.h>
#include <memory.h>

/**
	@brief Map  ages in the section range

	@note
		Ŀǰ�����߱�֤���ຯ����������
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
	
	/* ����Ҫflush tlb����Ϊ��ӳ���page����tlb�� */
end:
	return true;
	
err:
	TODO("Cleanup PTE for failed mapping");
	return false;
}

/**
	@brief Create a page on VM 

	@note
		Ŀǰ�����߱�֤���ຯ����������

*/
bool km_page_create(struct km *mem_dst, unsigned long address, page_prot_t prot)
{
	pte_t entry;
	unsigned long page = 0;
	struct km_walk_ctx dst_ctx;

	KM_WALK_INIT(mem_dst, &dst_ctx);
	
	if (unlikely(km_walk_to(&dst_ctx, address) == false))
		goto err;

	/* Create ģʽӦ�ÿ������κ����ݣ����������ⱻ���˸ոմ���ɹ� */
	if (km_pte_read(&dst_ctx))
		goto end;
	page = km_page_alloc();
	if (!page)
		goto err;
	entry = page | km_arch_get_flags(prot);	
	km_pte_write(&dst_ctx, entry);
	
	/* ����Ҫflush tlb����Ϊ�µ�page��tlb���ǲ����ڵ� */
end:
	return true;

err:
	if (page)
		km_page_dealloc(page);
	return false;
}

/**
	@brief ����ҳ
*/
int km_page_share(struct km *dst, unsigned long dst_addr, struct km *src, unsigned long src_addr)
{
	unsigned long i, src_end = src_addr + PAGE_SIZE;
	int r = KM_PAGE_SHARE_RESULT_ERR;
	page_prot_t prot;
	unsigned long old;		
	unsigned long va_dst;
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
		r = KM_PAGE_SHARE_RESULT_OK;
		goto end;		
	}

 	/* Write entry one by one */
 	for (i = src_addr; i < src_end; i += PAGE_SIZE)
 	{
		pte_t pte;

		/* Read src one by one */
		if (unlikely(km_pte_read_and_next(&src_ctx, &pte) == false))
			goto end;

		/* Not a valid pte */
		if (KE_PAGE_INVALID_PTE(pte))
		{
			printk("invalid src pte %x, i = %d.\n", pte, i);
			r = KM_PAGE_SHARE_RESULT_SRC_INVALID;
			goto end;
		}

		/* Write the pte to dst, just copy the the src's flags */
		if (unlikely(km_pte_write_and_next(&dst_ctx, pte | PAGE_FLAG_FROM_OTHER) == false))
			goto end;
 	}
 	r = KM_PAGE_SHARE_RESULT_OK;
	
	/* ����Ҫflush tlb����Ϊshare��page��tlb��ǰ�ǲ����ڵġ�*/
	
end:
	return r;
}