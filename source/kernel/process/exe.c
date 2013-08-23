/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   进程可执行对象
*/

#include <exe.h>
#include <memory.h>

#include "object.h"
static struct ko_process *init_process, *kernel_file_process;

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{

}

static struct cl_object_ops object_ops = {
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

static struct cl_object_type exe_type = {
	.name		= "可执行对象",
	.size		= sizeof(struct ko_exe),
	.ops		= &object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief 创立一个空的进
*/
struct ko_exe *kp_exe_create(struct ko_section *backend)
{
	struct ko_exe *p;
		
	p = cl_object_create(&exe_type);
	if (!p) goto err;	

	cl_object_inc_ref(backend);
	p->backend = backend;
	return p;
	
err1:
	cl_object_close(p);
err:
	return NULL;
}

void kp_exe_bind(struct ko_process *who, struct ko_exe *what)
{

}

void kp_exe_init()
{
	cl_object_type_register(&exe_type);

}
