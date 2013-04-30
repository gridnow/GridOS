/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内存对象管理器
 */

#include <types.h>

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

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, unsigned long size)
{
	struct ko_section *p;//printk("%s %s %d.\n", __FILE__, __FUNCTION__, __LINE__);
	
	p = cl_object_create(&section_type);
	if (!p) goto err;
	
	p->node.size = size;
	p->node.start = base;
	if (km_vm_create(where, &p->node) == false) goto err1;
	
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

void __init ks_init()
{
	cl_object_type_register(&section_type);
}

//------------test-----------------
void kernel_test()
{
	struct ko_section *ks;
	printk("\nTesting kernel...");
	
	ks = ks_create(kp_get_system(), KS_TYPE_PRIVATE, 0, 1000);
	printk("\n    ks = %p, virtual start = %p, size = %d.\n", ks, ks->node.start, ks->node.size);
}