/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程线程管理
*/

#include "thread.h"
#include "process.h"
#include "memory.h"

#include <string.h>

static struct ko_thread *init_thread;

static asmregparm void kernel_thread_fate_entry(unsigned long (*thread_entry)(unsigned long),unsigned long para)
{
	if (thread_entry)
		ka_call_dynamic_module_entry(thread_entry, para);
	//TODO delete the kernel thread
}

static bool thread_close(struct cl_object *obj)
{

}

static struct cl_object_ops thread_object_ops = {
	.close				= thread_close,
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
	.size		= sizeof(struct ko_thread),
	.ops		= &thread_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

/**
	@brief Create a thread
*/
struct ko_thread * kt_create(struct ko_process * where, void * wrapper, void * func, unsigned long param, unsigned long flags, xstring name)
{
	struct kt_thread_creating_context ctx = {0};
	struct ko_thread * p;

	/* Create the thread from micro layer */
	if (!wrapper || !func) goto err0;
	ctx.thread_entry 	= (unsigned long)func;
	ctx.para		 	= param;	
	ctx.on			 	= where;
	ctx.fate_entry		= (unsigned long)wrapper;
	if (where->cpl == KP_USER)
	{		
		ctx.fate_entry = (unsigned long)wrapper;
		ctx.stack_pos	 = 0;
		//TODO create stack
	}
	else
	{
		if (!wrapper) wrapper = kernel_thread_fate_entry;
		ctx.fate_entry = (unsigned long)wrapper;
		ctx.stack_pos	 = 0;
		//TODO create stack
	}		

	/* Has cmdline param? */
	if (flags & KT_CREATE_STACK_AS_PARA)
	{
		/* Write the cmdline to kernel stack(底部) for user to fetch */
		if (strlen((char*)param) >= KT_ARCH_THREAD_CP0_STACK_SIZE)
			goto err1;
		strcpy((char*)ctx.stack0, (char*)param);

		/* Write the real thread param for this address, so we know where to fetch */
		ctx.para = ctx.stack0;
	}
	
	p = cl_object_create(&thread_type);
	if (!p)	goto err1;
	kt_arch_init_thread(p, &ctx);
	return p;

err1:
	
err0:
	return NULL;
}

bool kt_init()
{
	/* The first thread in system, which is the idle thread for BSP */
	cl_object_type_register(&thread_type);
	init_thread = cl_object_create(&thread_type);
	if (!init_thread) 
		goto err;

	
	return true;
err:
	return false;
}
