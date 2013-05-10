/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   �����ַ������
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
	
	/* ����Ҫ��λ����λ�ã��Զ������������ڲ��ң��ֶ��������ض�λ���� */
	if (desired)
		start = desired;
	else
		start = range_start;
	
	list_for_each(section_list, usage_list)
	{
		pS = list_entry(section_list, struct km_vm_node, node);
		
		/* ָ����ַ������£�PS �� PP ֮��Ӧ�����µ�ַ���ڵĵط����ж��Ƿ����㹻�Ŀ�϶ */
		if (pS->start > start)												 				// pS ���µ�ַ�ұߵĽڵ�
		{
			if (start + size <= pS->start)													// PS û�к��µ�ַ��ͷ���ص�
			{
				if (pP)
				{
					if (pP->start + pP->size <= start)										// PP û�к��µ�ַ��β���ص�
						break;																// ��ô����һ����ȷ�ĵ�ַ
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
	
	/* ���һȦ�����ˣ���ô��ʾû���ҵ����ʵĲ���� */
	if (pP == pS)
	{
		/* �����Ǹ��յ������϶���û��PS */
		if (pP == NULL && pS == NULL)
			goto check_limit;
		
		/* �����ҵ�����ˣ���start���PSû���ҵ��������ܳ������� */
		if (start >= pS->start + pS->size)
			goto check_limit;
		
		/* ������ô����ʧ�ܵ� */
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
	unsigned long size = ALIGN(node->size, PAGE_SIZE);

	km_get_vm_range(where->cpl, &range_start, &range_len);

	spin_lock(&where->vm_list_lock);
	start = alloc_virtual_space(&where->vm_list, range_start, range_len, node->start, size);
	if (start == NULL) goto end;
	node->start = start;
	node->size = size;
	
	/* Actually insertion will not fail, or the alloc_virtual_space has BUG */
	if (insert_virtual_space(&where->vm_list, node, false) == false)
		goto end;
	
end:
	spin_unlock(&where->vm_list_lock);
	
	if (start == NULL)
		return false;
	
	return true;
}

