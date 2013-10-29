/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   虚拟地址管理器
 */
#include <kernel/ke_memory.h>

#include <types.h>
#include <vm.h>
#include <process.h>
#include <section.h>
#include <page.h>

#include "debug.h"

/**
	Find address for with len inside start + total_len.
 */
static unsigned long alloc_virtual_space(struct list_head * usage_list,
								   unsigned long range_start, unsigned long range_len,
								   unsigned long desired, unsigned long size)
{
	struct list_head * section_list;
	struct km_vm_node *pS = NULL, *pP = NULL;
	unsigned long start = NULL;
	bool found = true;
	
	/* 首先要定位到的位置，自动的则在区间内部找，手动的则在特定位置找 */
	if (desired)
		start = desired;
	else
		start = range_start;
	
	list_for_each(section_list, usage_list)
	{
		pS = list_entry(section_list, struct km_vm_node, node);
		//printk("ps->Start = %x, start = %x \n", pS->start, start);
		/* 指定地址的情况下，PS 与 PP 之间应该是新地址所在的地方，判断是否有足够的空隙 */
		if (pS->start > start)												 				// pS 是新地址右边的节点
		{
			if (start + size <= pS->start)													// PS 没有和新地址在头部重叠
			{
				if (pP)
				{
					if (pP->start + pP->size <= start)										// PP 没有和新地址在尾部重叠
						break;																// 那么就是一个正确的地址
				}
				else
					break;
			}
		}
		
		pP = pS;
		
		/* If is auto, set the start to the end of this section to find next hole */
		if (!desired)
		{
			unsigned long next = pS->start + pS->size;
			
			/* But if next dose not get the point of range start ? */
			if (next <= start)
				continue;
			
			/* Ok, fall to the [range_start, range_len] */
			start = next;
			if (start + size > range_start + range_len)
				break;
		}
	}
	
	/* 如果一圈找完了，那么表示没有找到合适的插入点 */
	if (pP == pS)
	{
		/* 根本是个空的链表，肯定是没有PS */
		if (pP == NULL && pS == NULL)
			goto check_limit;
		
		/* 可能找到最后了，比start大的PS没有找到，追加到最后。那么看看超过限制没 */
		if (start >= pS->start + pS->size)
			goto check_limit;
		
		/* 否则，那么都是失败的 */
		goto err1;
	}	
check_limit:	
	if (start + size <= range_len + range_start /* In limit */ && start >= range_start)
		goto ok;
	else
		goto err1;
	
ok:
	return start;
	
err1:
	return NULL;
}

static bool insert_virtual_space(struct list_head * list, struct km_vm_node * who, bool sub)
{
	struct list_head *t;
	bool inserted = false;
	struct km_vm_node *p;

	list_for_each(t, list)
	{
		p = list_entry(t, struct km_vm_node, node);		
		if (p->start > who->start && p->start >= who->start + who->size)
		{
			if (likely(sub == false))
				list_add_tail(&who->node, t);
			else
				list_add_tail(&who->subsection_link, t);
			inserted = true;
			break;
		}
	}
	
	if (unlikely(inserted == false))
	{
		/* 空表？也可能直接要求在链表的最后，因为上面的循环没有匹配成功 */
		if (list_empty(list) || who->start >= p->start + p->size)
		{
			if (likely(sub == false))
				list_add_tail(&who->node, list);
			else
				list_add_tail(&who->subsection_link, list);
			inserted = true;
		}
	}
	
	return inserted;
}

void get_vm_range(int process_cpl, unsigned long *start, unsigned long *size, unsigned long *desired_start, unsigned long *desired_size, bool is_type_kernel)
{	
	switch (process_cpl)
	{
		case KP_CPL0:
			if (start)
				*start = HAL_GET_BASIC_KADDRESS(0) + CONFIG_HAL_KERNEL_MEM_LEN;
			if (size)
				*size = CONFIG_HAL_KERNEL_VM_LEN - PAGE_SIZE;
			break;
			
		case KP_CPL0_FAKE:
		case KP_USER:
		{
			unsigned long user_start = 0x200000;
			if (start)
				*start = user_start;
			if (size)
			{
#if defined(__i386__) || defined (__arm__)
				*size = HAL_GET_BASIC_KADDRESS(0) - user_start;
				if (is_type_kernel)
				{
					*desired_size = *size;
					*desired_start = HAL_GET_BASIC_KADDRESS(0);
					*start = *desired_start;
				}
#elif defined(__mips64__)
				*size = 1024 * 1024 * 1024 * 1024;
				if (is_type_kernel)
				{
					*desired_size = *size;
					*desired_start = HAL_GET_BASIC_KADDRESS(0);
					*start = *desired_start;
				}
#else
#error "Platform must be defined for get_vm_range"
#endif
			}
			break;
		}
		default:
			BUG();
	}
	
	*desired_size = ALIGN(*desired_size, PAGE_SIZE);
}

bool km_vm_create(struct ko_process *where, struct km_vm_node *node, int is_type_kernel)
{
	bool r = false;
	unsigned long range_start, range_len;

	get_vm_range(where->cpl, &range_start, &range_len, &node->start, &node->size, is_type_kernel);

	KP_LOCK_PROCESS_SECTION_LIST(where);
	
	if ((node->start = alloc_virtual_space(&where->vm_list, range_start, range_len, node->start, node->size)) == NULL)
	{
	//	printk("node start = %x, node size = %x.\n", node->start, node->size);
		goto end1;
	}
	
	/* Actually insertion will not fail, or the alloc_virtual_space has BUG */
	if (insert_virtual_space(&where->vm_list, node, false) == false)
	{
	//	printk("insert error.\n");
		goto end2;
	}
	
	r = true;
	
end2:
	//TODO delete the virtual space already allocated
end1:
	KP_UNLOCK_PROCESS_SECTION_LIST(where);
	return r;
}

/**
	@brief Allocate space in a section
*/
unsigned long km_vm_create_sub(struct ko_process *where, struct km_vm_node *parent, struct km_vm_node *sub_node, unsigned long start, unsigned long size)
{
	unsigned long base;
	unsigned long range_start, range_size;
	unsigned long flags;

	range_size = parent->size;
	range_start = parent->start;

	/* The list of sub-section on ks must be locked */
	KP_LOCK_PROCESS_SECTION_LIST(where);

	/* Check or Allocate the space */
	//printk("Range for sub %d@%x, sub addr %d@%x.\n", range_size, range_start, size, start);
	base = alloc_virtual_space(&parent->subsection_head, range_start, range_size, start, size);
	if (!base) goto end;

	/* Set the information to the object */
	sub_node->size  = size; 
	sub_node->start = base;

	/* Insert to the thread list */
	insert_virtual_space(&parent->subsection_head, sub_node, true);
	
end: 
	/* Unlock the process's section list */
	KP_UNLOCK_PROCESS_SECTION_LIST(where);

	return base;
}

/******************************************************
 Interface
 ******************************************************/
void *km_map_physical(unsigned long physical, unsigned long size, unsigned long flags)
{
	struct ko_section *ks;
	unsigned long base = 0;
	page_prot_t map_flags = KM_MAP_DEVICE;
	
	if (flags & KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL)
		base = flags & PAGE_MASK;
	if (flags & KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE)
		map_flags = KM_PROT_READ | KM_PROT_WRITE;

	ks = ks_create(kp_get_system(), KS_TYPE_DEVICE, base, size, map_flags);
	if (!ks)
		goto err;	
	
	if (km_page_map_range(&kp_get_system()->mem_ctx, ks->node.start,
						  ks->node.size, physical >> PAGE_SHIFT, map_flags) == false)
		goto err1;
	
	return (void*)ks->node.start;
	
err1:
	ks_close(ks);
err:
	return NULL;
}

void *km_alloc_virtual(unsigned long size, page_prot_t prot)
{
	struct ko_section *ks;
	unsigned long base = 0;
	
	ks = ks_create(kp_get_system(), KS_TYPE_PRIVATE, base, size, prot);
	if (!ks)
		goto err;
	
	return (void*)ks->node.start;
	
err:
	return NULL;
}

void km_dealloc_virtual(void *kv)
{
	//TODO
	TODO("");
}
