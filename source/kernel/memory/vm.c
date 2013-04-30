/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   虚拟地址管理器
 */

#include <types.h>
#include "vm.h"
#include "process.h"

#include "hal_config.h"
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
		
		/* 可能找到最后了，比start大的PS没有找到，并可能超过限制 */
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
	
	list_for_each(t, list)
	{
		struct km_vm_node * p = list_entry(t, struct km_vm_node, node);
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
	
	if (unlikely(inserted == false && list_empty(list)))
	{
		if (likely(sub == false))
			list_add_tail(&who->node, list);
		else
			list_add_tail(&who->subsection_link, list);
		inserted = true;
	}
	
	return inserted;
}

void km_get_vm_range(int process_cpl, unsigned long *start, unsigned long *size)
{
	switch (process_cpl)
	{
		case KP_CPL0:
			if (start)
				*start = HAL_GET_BASIC_KADDRESS(0) + CONFIG_HAL_KERNEL_MEM_LEN;
			if (size)
				*size = CONFIG_HAL_KERNEL_VM_LEN - PAGE_SIZE;
			break;
			
		case KP_USER:
		{
			unsigned long user_start = 0x200000;
			if (start)
				*start = user_start;
			if (size)
			{
#if defined(__i386__) || defined (__arm__)
				*size = HAL_GET_BASIC_KADDRESS(0) - user_start;
#elif defined(__mips64__)
				*size = 1024 * 1024 * 1024 * 1024;
#endif
			}
			break;
		}
		default:
			BUG();
	}
}

bool km_vm_create(struct ko_process *where, struct km_vm_node *node)
{
	unsigned long range_start, range_len;
	unsigned long start = NULL;

	km_get_vm_range(where->cpl, &range_start, &range_len);
		
	spin_lock(&where->vm_list_lock);
	start = alloc_virtual_space(&where->vm_list, range_start, range_len, node->start, node->size);
	if (start == NULL) goto end;
	node->start = start;
	
	/* Actually insertion will not fail, or the alloc_virtual_space has BUG */
	if (insert_virtual_space(&where->vm_list, node, false) == false)
		goto end;
	
end:
	spin_unlock(&where->vm_list_lock);
	
	if (start == NULL)
		return false;
	
	return true;
}

