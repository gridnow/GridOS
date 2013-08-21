/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   Exception handling
*/

#include <section.h>
#include <thread.h>
#include <process.h>

#include <page.h>

/**
	@brief The handler type for exception handler
*/
typedef bool (*refill_handler)(struct ko_thread * current, struct ko_section * where, unsigned long address, unsigned long code);
static refill_handler exception_handler[KS_TYPE_MAX];

static bool refill_null(struct ko_thread * current, struct ko_section * where, unsigned long address, unsigned long code)
{
	printk("Uninited refill handler.\n");
	return false;
}

/**
	@brief Private memory 
*/
static bool refill_private(struct ko_thread * current, struct ko_section * where, unsigned long address, unsigned long code)
{
	bool r = false;
	struct km *mem = NULL;
	
	/* R/W violation */
	if (code & PAGE_FAULT_P)
	{
		//TODO
		goto end;
	}
	
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

struct ko_section * ks_get_by_vaddress_unlock(struct ko_process * where, unsigned long address)
{
	struct ko_section *ks = NULL;
	struct km_vm_node *p;
	struct list_head *t;
	
	/* Loop each section */
	list_for_each(t, &where->vm_list)
	{
		p = list_entry(t, struct km_vm_node, node);
		/* The address must in the section limit */
		if (p->start <= address && address < p->start + p->size)
		{
			//printk("address = %x, start %x, size %x.\n", address, p->start, p->size);
			ks = (struct ko_section*)p;
			break;
		}
		
		/* If section is behind the address, not need to find */
		if (p->start > address)
			break;
	}
	
	return ks;
}

struct ko_section * ks_get_by_vaddress(struct ko_process * where, unsigned long address)
{
	struct ko_section * ks;
	
	KP_LOCK_PROCESS_SECTION_LIST(where);
	ks = ks_get_by_vaddress_unlock(where, address);
	KP_UNLOCK_PROCESS_SECTION_LIST(where);
	
	return ks;
}

/**
	@brief Exception entry
*/
bool ks_exception(struct ko_thread *thread, unsigned long error_address, unsigned long code)
{
	struct ko_thread * current = thread;
	struct ko_section * ks = ks_get_by_vaddress(KT_GET_KP(current), error_address); 
	if (!ks) goto err1;

// 	printk("线程名 %s:", KO_GET_NAME_SIMPLE(current));	
// 	printk("线程%p发生了地址%p异常，尝试修复Section %p(%s), &ks = %x.\n", current, error_address, ks, ks_type_name(ks), &ks);
	

	/* Call it handler */
	return exception_handler[ks->type & KS_TYPE_MASK](current, ks, error_address, code);

err1:
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
//	exception_handler[KS_TYPE_EXE]		= refill_exe;
	exception_handler[KS_TYPE_PRIVATE]	= refill_private;
//	exception_handler[KS_TYPE_STACK]	= refill_normal;
//	exception_handler[KS_TYPE_FILE]		= refill_file;
//	exception_handler[KS_TYPE_SHARE]	= refill_share;
//	exception_handler[KS_TYPE_KERNEL]	= refill_kernel;

	return true;
}

