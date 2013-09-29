/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内存对象管理器
 */

#include <types.h>
#include <stdarg.h>

#include "object.h"

#include "memory.h"
#include "process.h"
#include "section.h"

static bool object_close(real_object_t *obj)
{
	
}

static void object_init(real_object_t *obj)
{
	struct ko_section *p = (void*)obj;
	INIT_LIST_HEAD(&p->node.subsection_head);
}

static struct cl_object_ops section_object_ops = {
	.close				= object_close,
	.init				= object_init
};

static bool alloc_space(struct cl_object_type *type, void **base, size_t *size, enum cl_object_memory_type memory_type)
{
	void *object_memory;
	int count = 0;
	
	switch (memory_type)
	{
		case COMMON_OBJECT_MEMORY_TYPE_NAME:
		case COMMON_OBJECT_MEMORY_TYPE_OBJ:
		case COMMON_OBJECT_MEMORY_TYPE_NODE:
			count = 1;
			break;
	}
	if (count == 0) goto err;
	
	object_memory = km_page_alloc_kerneled(count);
	if (!object_memory) goto err;
	*base = object_memory;
	*size = count * PAGE_SIZE;
	return true;
	
err:
	return false;
};

static void free_space(struct cl_object_type *type, void *base, size_t size, enum cl_object_memory_type memory_type)
{
	
}

static struct cl_object_type section_type = {
	.name		= "内存对象",
	.size		= sizeof(struct ko_section),
	.ops		= &section_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, unsigned long size, page_prot_t prot)
{
	struct ko_section *p;
	
	if (size == 0) goto err;	
	p = cl_object_create(&section_type);
	if (!p)	goto err;	

	p->type 		= type;
	p->node.size 	= size;
	p->node.start 	= base;
	p->prot			= prot;
	if (km_vm_create(where, &p->node) == false)
		goto err1;
	
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

void ks_close(struct ko_section *ks)
{
	cl_object_close(ks);
}

/**
	@brief Create a sub node on the current section
*/
struct ko_section * ks_sub_create(struct ko_process * who, struct ko_section * where, unsigned long sub_address, unsigned long sub_size)
{
	struct ko_section * sub;

	/* Create the sub-section and type is set to normal */
	sub = cl_object_create(&section_type);
	if (!sub) goto err1;
	sub->type = KS_TYPE_PRIVATE;
	sub->prot = KM_PROT_READ;

	/* Create the base */
	sub_address = km_vm_create_sub(who, &where->node, &sub->node, sub_address, sub_size);
	if (!sub_address) goto err2;

	return sub;

err2:
	ks_close(sub);
err1:
	return NULL;
}

/**
	@brief Locate sub

	@note
		This is an unlock version. parent 不动，subsection的链表也没有去修改，是吧？
*/
struct ko_section *ks_sub_locate(struct ko_section * where, unsigned long address)
{
	struct list_head * list;
	struct km_vm_node *node = &where->node, *sub;

	list_for_each(list, &node->subsection_head)
	{
		sub = list_entry(list, struct km_vm_node, subsection_link);		
		if (address >= sub->start && address < sub->start + sub->size)
			return KM_VM_NODE_TO_SECTION(sub);
	}

	return NULL;
}

/**
	@brief Close the subsection of a section
*/
void ks_sub_close(struct ko_process * who, struct ko_section * which)
{
	struct list_head * list, * n;
	struct km_vm_node *node = &which->node, *sub;

	list_for_each_safe(list, n, &node->subsection_head)
	{
		sub = list_entry(list, struct km_vm_node, subsection_link);
		list_del_init(&sub->node);
		ks_close(KM_VM_NODE_TO_SECTION(sub));
	}
}

void ks_init_for_process(struct ko_process *who)
{
	INIT_LIST_HEAD(&who->vm_list);
	ke_spin_init(&who->vm_list_lock);
}

void __init ks_init()
{
	cl_object_type_register(&section_type);
	
	/* Trim the memory mapping, i386 is full mapping */
	km_arch_trim(); 
	ks_exception_init();
	km_valloc_init();
}

//------------test-----------------
#include <kernel/ke_event.h>
#include <thread.h>
struct ke_event ev;
static void test_thread(unsigned long para)
{
	printk("test thread. %d...", para);
	while (1)
		kt_schedule();
}

void kernel_test()
{
	int i;
	
	//for (i = 0; i < 34; i++)
	//	kt_create_kernel(test_thread, i);
	fss_main();

	/* we have timer tick to switch preemptly */
#if 0
	/* Startup first disk file */
	while (1)
		kt_schedule();
#endif
}