/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Sihai
*   Exception handling
*/

#include <section.h>
#include <thread.h>
#include <process.h>
#include <memory.h>
#include <exe.h>

#include <string.h>

#include <page.h>

#include "../source/subsystem/fs/include/fss.h"

/**
	@brief The handler type for exception handler
*/
typedef bool (*refill_handler)(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code);
static refill_handler exception_handler[KS_TYPE_MAX];

static struct ko_section *ks_get_by_vaddress_unlock(struct ko_process *where, unsigned long address)
{
	struct ko_section *ks = NULL;
	struct km_vm_node *p;
	struct list_head *t;
	
	//TODO:REFINE: 红黑树优化
	
	/* Loop each section */
	list_for_each(t, &where->vm_list)
	{
		p = list_entry(t, struct km_vm_node, node);
		//printk("address = %x, start %x, size %x.\n", address, p->start, p->size);
		
		if (p->start <= address && address < p->start + p->size)
		{
			//printk("address = %x, start %x, size %x.\n", address, p->start, p->size);
			ks = (struct ko_section*)p;
			
			break;
		}
		
		if (p->start > address)
			break;
	}
	
	return ks;
}

 struct ko_section *ks_get_by_vaddress(struct ko_process *where, unsigned long address)
{
	struct ko_section *ks;
	
	KP_LOCK_PROCESS_SECTION_LIST(where);
	ks = ks_get_by_vaddress_unlock(where, address);
	KP_UNLOCK_PROCESS_SECTION_LIST(where);
	
	return ks;
}

static bool restore_file(struct ko_process *who, struct ko_section *where, unsigned long address)
{
	void *db_addr;
	uoffset pos;
	bool ret = false;
	struct km *mem_src, *mem_dst;

	pos = address - where->node.start;
	db_addr = fss_map_prepare_dbd(where->priv.file.file, who, pos);
	if (db_addr == NULL)
		goto end;

	mem_dst = kp_get_mem(who);
	mem_src = kp_get_mem(kp_get_system());

	if (km_page_share(mem_dst, address, mem_src, (unsigned long)db_addr, KM_PROT_READ) != KM_PAGE_SHARE_RESULT_OK)
		goto end1;

	ret = true;

end1:
	kp_put_mem(mem_dst);
	kp_put_mem(mem_src);
end:
	return ret;
}

static bool ks_restore(struct ko_process *who, struct ko_section *where, unsigned long address)
{
	switch(where->type & KS_TYPE_MASK)
	{
		case KS_TYPE_FILE:
			return restore_file(who, where, address);
		default:
			TRACE_UNIMPLEMENTED("");
	}
	
	return false;
}

static bool refill_null(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	printk("Uninited refill handler.\n");
	return false;
}

static bool refill_share(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	unsigned long source_address;
	struct ko_section *src = where->priv.share.src;
	
	source_address = where->priv.share.offset + src->node.start;
	source_address += address - where->node.start;
	
	return ks_restore_share(KT_GET_KP(current), where,
					 where->priv.share.src_process, src,
					 address, source_address, 0);
}

static bool refill_exe(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	bool cow, r = false;
	struct km *mem;
	struct ko_section *detailed;

	/* Get sub section */
	detailed = ks_sub_locate(where, address);
	if (unlikely(!detailed))
		goto end;

	/*
		But the address may exceed the shared range, it must be a BSS like segment. 
		The first BSS page may have some valid data from merged data&bss page, but it is handled at image relocating, so this will not meet this condition.
	*/
	if (unlikely(address >= detailed->node.start/*mapped base*/ + detailed->priv.share.size/*file size*/))
	{
//		unsigned long phy;

		//printk("    BSS like segment virtual base %p, address %p \n", detailed->node.start, address);

		/* 
			TODO: Optimize: If just read, a common zero page from system can be mapped until the page is written.
		*/

		/* Have no source address to map, create page with zero filled and RW mode(so no COW) */
		mem = kp_get_mem(KT_GET_KP(current));
		r = km_page_create(mem, address, KM_PROT_READ | KM_PROT_WRITE);
		kp_put_mem(mem);

		/* 由于是缺页异常，那么经过page写入就可以立即访问该页了，无需刷新TLB */
		if (r == true)
			memset((void*)KM_PAGE_ROUND_ALIGN(address), 0, PAGE_SIZE);	
		//printk("EXE: write zero page %x\n", address);
	}
	else
	{
		cow = false;

		/* Caused by write may trigger COW */
		if (code & PAGE_FAULT_W)
		{
			if (!(detailed->prot & KM_PROT_WRITE))
			{
				/* 
					In the process of image relocation, TEXT segment may be written.
					But in when we are not relocation, writing to TEXT segment should not be permitted. 
				*/
				//TODO: To handle these two cases for Text Segment Writing.
				//goto err1;
			}

			/* But the page is present? Non present require to construct the relation first */
			if (code & PAGE_FAULT_P)
				cow = true;
		}

		if (cow == false)
			r = kp_exe_share(KT_GET_KP(current), detailed, address, where->priv.exe.exe_object);
		else
		{
		//	printk("Cow at %p.\n", address);
			mem = kp_get_mem(KT_GET_KP(current));
			r = km_page_create_cow(mem, address);
			kp_put_mem(mem);
		}
	}
	
end:
	return r;
}

/**
	@brief Private memory 
*/
static bool refill_private(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	bool r = false;
	struct km *mem = NULL;
	
	/* R/W violation */
	if (code & PAGE_FAULT_P)
	{
		//TODO
		goto end;
	}
	
	/* Get the address key, and it may be fixed by other thread */
	mem = kp_get_mem(KT_GET_KP(current));
	if(km_page_create(mem, address, where->prot) == false)
		goto end;
	if (where->prot & KM_MAP_ANON)
	{
		/* The new page is not flushed to the TLB, so we use physical to access it */
		//memset((void*)HAL_GET_BASIC_KADDRESS(phy), 0, PAGE_SIZE);
		TODO("clear page");
	}
	r = true;

end:
	if (mem)
		kp_put_mem(mem);
	return r;
}

static bool refill_file(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	bool r = false;

	if (code & PAGE_FAULT_W)
	{
		if (!(where->prot & KM_PROT_WRITE))
			goto end;
		//TODO("COW");
		//goto end;
	}

	r = restore_file(KT_GET_KP(current), where, address);

end:
	return r;
}

static bool refill_kernel(struct ko_thread *current, struct ko_section *where, unsigned long address, unsigned long code)
{
	struct km *cur_mem;
	struct ko_section *ks;
	
	if ((ks = ks_get_by_vaddress(kp_get_system(), address)) == NULL)
		goto err;
	if (exception_handler[ks->type & KS_TYPE_MASK](current, ks, address, 0) == false)
		goto err;
#if 0
	if (ks_restore(kp_get_system(), ks, address) == false)
		goto err;
#endif
	cur_mem = kp_get_mem(KT_GET_KP(current));
	km_page_share_kernel(cur_mem, address);
	kp_put_mem(cur_mem);
	
	return true;
	
err:
	return false;
}

/**
	@brief Exception entry
*/
bool ks_exception(struct ko_thread *thread, unsigned long error_address, unsigned long code)
{
	bool ret;
	struct ko_section *ks;
	struct ko_process *where = KT_GET_KP(thread);
	
	ks = ks_get_by_vaddress(where, error_address);
	if (ks)
		ret = exception_handler[ks->type & KS_TYPE_MASK](thread, ks, error_address, code);
	else
		ret = false;
	
	return ret;
}

bool ks_restore_share(struct ko_process *dst_process, struct ko_section *dst_section,
					  struct ko_process *src_process, struct ko_section *src_section,
					  unsigned long to, unsigned long src, page_prot_t prot_overwrite)
{
	int ret;
	struct km *dst_mem, *src_mem;
	page_prot_t prot;
	
	/*
		In EXE case, we must assign read mode first for COW.
	*/
	if (prot_overwrite)
		prot = prot_overwrite;
	else
		prot = dst_section->prot;
	
share_again:
	dst_mem = kp_get_mem(dst_process);
	src_mem = kp_get_mem(src_process);
	ret = km_page_share(dst_mem, to,
						src_mem, src,
						prot);
	kp_put_mem(dst_mem);
	kp_put_mem(src_mem);
	
	if (ret != KM_PAGE_SHARE_RESULT_OK)
	{
		if (ret == KM_PAGE_SHARE_RESULT_ERR)
			goto err;
		else if (ret == KM_PAGE_SHARE_RESULT_SRC_INVALID)
		{
			if (ks_restore(src_process, src_section, src) == false)
				goto err;
			goto share_again;
		}
	}
	
	return true;
	
err:
	return false;
}

/**
	@brief Init the exception module
*/
bool __init ks_exception_init()
{
	int i;

	/* For unset handler */
	for (i = 0; i < KS_TYPE_MAX; i++)
		exception_handler[i] = refill_null;

	/* For specified type */
	exception_handler[KS_TYPE_EXE]		= refill_exe;
	exception_handler[KS_TYPE_PRIVATE]	= refill_private;
	exception_handler[KS_TYPE_STACK]	= refill_private;
	exception_handler[KS_TYPE_FILE]		= refill_file;
	exception_handler[KS_TYPE_SHARE]	= refill_share;
	exception_handler[KS_TYPE_KERNEL]	= refill_kernel;
	return true;
}

