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
	int i = 0, j;
	
	if (para == 0)
	{
		ke_event_init(&ev, false, true);
	}
	return;

	while(1)
	{
		i++;

		printk("\nWaiting(%d)...", para);
		j = ke_event_wait(&ev, 1000);
		printk("wait(%d) result = %d...", para, j);
		ke_event_set(&ev);
		
	}
}

void kernel_test()
{
	int i;
	
	//for (i = 0; i < 10; i++)
	//	kt_create_kernel(test_thread, i);
	fss_main();

	/* Startup first disk file */
	while (1)
		kt_schedule();
}