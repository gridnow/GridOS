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

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, size_t size)
{
	struct ko_section *p;
	
	if (size == 0) goto err;	
	p = cl_object_create(&section_type);
	if (!p)	goto err;	

	p->type 		= type;
	p->node.size 	= size;
	p->node.start 	= base;
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
}

/******************************************************
Interface
******************************************************/
void *km_map_physical(unsigned long physical, size_t size, unsigned long flags)
{
	struct ko_section *ks;
	unsigned long base = 0;
	unsigned long map_flags = KM_MAP_DEVICE;
	
	if (flags & KM_MAP_PHYSICAL_FLAG_WITH_VIRTUAL)
		base = flags & PAGE_MASK;
	
	ks = ks_create(kp_get_system(), KS_TYPE_DEVICE, base, size);
	if (!ks)
		goto err;
	
	if (flags & KM_MAP_PHYSICAL_FLAG_NORMAL_CACHE)
		map_flags = KM_PROT_READ | KM_PROT_WRITE;
	
	if (km_page_map_range(&kp_get_system()->mem_ctx, ks->node.start,
					ks->node.size, physical >> PAGE_SHIFT, map_flags) == false)
		goto err1;
	
	return (void*)ks->node.start;
	
err1:
	ks_close(ks);
err:
	return NULL;
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
	
	/* Start the kernel thread recaller */
	printk("kernel_test switch thread loop\n");
	while (1)
		kt_schedule();
	
}