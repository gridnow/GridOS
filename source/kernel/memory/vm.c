/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   �����ַ������
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
	
	/* ����Ҫ��λ����λ�ã��Զ������������ڲ��ң��ֶ��������ض�λ���� */
	if (desired)
		start = desired;
	else
		start = range_start;

	//printk("start %x, desired %x, range start %x.\n", start, desired, range_start);
	list_for_each(section_list, usage_list)
	{
		pS = list_entry(section_list, struct km_vm_node, node);
		//printk("ps->Start = %x(%dkb), start = %x, desired size = %dkb \n", pS->start, pS->size / 1024, start, size / 1024);
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
		
		/* �����ҵ�����ˣ���start���PSû���ҵ���׷�ӵ������ô������������û */
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
		/* �ձ�Ҳ����ֱ��Ҫ��������������Ϊ�����ѭ��û��ƥ��ɹ� */
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
			unsigned long user_start = 0x400000;
			if (start)
				*start = user_start;
			if (size)
			{
#if defined(__i386__) || defined (__arm__)
				*size = HAL_GET_BASIC_KADDRESS(0) - user_start;
				/* For big kernel sharing creation */
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

bool km_vm_create(struct ko_process *where, struct km_vm_node *node, unsigned long type)
{
	bool r = false;
	unsigned long range_start, range_len;
	int is_type_kernel = type == KS_TYPE_KERNEL || type == KS_TYPE_DEVICE;
	
	get_vm_range(where->cpl, &range_start, &range_len, &node->start, &node->size, is_type_kernel);

	KP_LOCK_PROCESS_SECTION_LIST(where);
	if ((node->start = alloc_virtual_space(&where->vm_list, range_start, range_len, node->start, node->size)) == NULL)
	{
		//printk("node start = %x, node size = %dkb, RANGE start = %x, size = %dkb.\n", node->start, node->size/1024,
		//	range_start, range_len / 1024);
		goto end1;
	}
	
	/* Actually insertion will not fail, or the alloc_virtual_space has BUG */
	if (insert_virtual_space(&where->vm_list, node, false) == false)
	{
		//printk("insert error.\n");
		goto end2;
	}
	
	r = true;
	
end2:
	//TODO delete the virtual space already allocated
end1:
	KP_UNLOCK_PROCESS_SECTION_LIST(where);
	return r;
}

void km_vm_delete(struct ko_process *where, struct km_vm_node *what)
{
	KP_LOCK_PROCESS_SECTION_LIST(where);
	/* INIT ���ϲ�֪����ַ�ǲ��Ǳ��ӵ�ַ�ռ��г��� */
	list_del_init(&what->node);
	KP_UNLOCK_PROCESS_SECTION_LIST(where);
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

	if ((ks = ks_create(kp_get_system(), KS_TYPE_DEVICE, base, size, map_flags)) == NULL)
		goto err;	
	if (km_page_map_range(&kp_get_system()->mem_ctx, ks->node.start,
						  ks->node.size, physical >> PAGE_SHIFT, map_flags) == false)
		goto err1;
	
	return (void*)ks->node.start;
	
err1:
	ks_close(kp_get_system(), ks);
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
