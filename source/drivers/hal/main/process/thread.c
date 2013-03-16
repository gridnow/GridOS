/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程线程管理
*/

#include "thread.h"
#include "memory.h"

static struct kt_thread init_thread;

static bool thread_close(struct cl_object *obj)
{

}

static void thread_init(struct cl_object *object)
{

}

static void *thread_sync_object(struct cl_object *obj)
{

}

static struct cl_object_ops thread_object_ops = {
	.close				= thread_close,
	.init				= thread_init,
	.get_sync_object	= thread_sync_object,
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

};

static struct cl_object_type thread_type = {
	.name		= "线程对象",
	.size		= sizeof(struct kt_thread),
	.ops		= &thread_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

void kt_init()
{
	void *p;
	printk("kt_init prepare creating thread object...");
	cl_object_type_register(&thread_type);
	p = cl_object_create(&thread_type);
	printk("ret p is %x.\n", p);
}