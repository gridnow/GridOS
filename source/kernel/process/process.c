/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   进程管理
*/

#include "process.h"
#include "memory.h"

#include "object.h"
static struct ko_thread *init_process;

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{
	struct ko_process *p = (struct ko_process *)obj;
	
}

static struct cl_object_ops process_object_ops = {
	.close				= object_close,
	.init				= object_init,
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

static struct cl_object_type process_type = {
	.name		= "进程对象",
	.size		= sizeof(struct ko_process),
	.ops		= &process_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief 获取系统进程对象
*/
struct ko_process *kp_get_system()
{
	return init_process;
}

/**
	@brief 创立一个空的进程对象
*/
struct ko_process *kp_create(int cpl, xstring name)
{
	struct ko_process *p;
	
	if (cpl != KP_CPL0 || cpl != KP_USER)
		goto err;
	p = cl_object_create(&process_type);
	if (!p) goto err;
	
	p->cpl = cpl;
	if (!cl_object_set_name(p, name))
		goto err1;
	
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

bool kp_init()
{
	/* The first thread in system, which is the idle thread for BSP */
	cl_object_type_register(&process_type);
	init_process = cl_object_create(&process_type);
	if (!init_process) 
		goto err;

	
	return true;
err:
	return false;
}

