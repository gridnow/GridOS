/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程管理
*/
#include <ystd.h>

#include <linkage.h>
#include <bitops.h>

#include <thread.h>
#include <section.h>
#include "memory.h"
#include "object.h"
#include "process.h"
#include "cpu.h"
#include <thread.h>

#include <string.h>

static struct ko_thread *init_thread;

#define ka_call_dynamic_module_entry(f, p) f(p)
static asmregparm __noreturn void kernel_thread_fate_entry(unsigned long (*thread_entry)(unsigned long),unsigned long para)
{
	if (thread_entry)
		ka_call_dynamic_module_entry(thread_entry, para);
	
	printk("Kernel level thread die ,entry %x.\n", thread_entry);
	kt_delete_current();
}

static bool object_close(void *by, real_object_t *obj)
{
	return true;
}

static bool object_init(real_object_t *obj)
{
	struct ko_thread *thread = (struct ko_thread *)obj;
	INIT_LIST_HEAD(&thread->queue_list);
	spin_lock_init(&thread->ops_lock);
	
	ktm_msg_init(thread);

	return true;
}

static struct cl_object_ops thread_object_ops = {
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

static struct cl_object_type thread_type = {
	.name		= "线程对象",
	.size		= sizeof(struct ko_thread),
	.ops		= &thread_object_ops,
	.add_space	= alloc_space,
	.free_space	= free_space,
};

static void init_idle_thread(struct kc_cpu *cpu, struct ko_thread **idle)
{
	struct ko_thread *p;
	
	p = cl_object_create(&thread_type);
	if (!p) 
		goto end;
	p->process = kp_get_system();
	cpu->cur = cpu->idle = p;
	kt_wakeup(p);

end:
	*idle = p;
}

/**
	@brief Create a thread
*/
struct ko_thread *kt_create(struct ko_process *where, struct kt_thread_creating_context *ctx)
{
	void *teb = NULL;
	struct ko_thread *p = NULL;
	
	/* Handle stack */
	ctx->cpl = where->cpl;
	if (ctx->cpl == KP_USER)
	{
		struct ko_section *ks;
		int stack_top, stack_size;
		
		//TODO: 一般，不是第一个线程无需这么大的堆栈
		stack_top = stack_size = KT_THREAD_FIRST_STACK_SIZE;
		/* Create the thread stack */
		if (NULL == (ks = ks_create(where, KS_TYPE_STACK, 0, stack_size, KM_PROT_READ|KM_PROT_WRITE)))
			goto err0;
		
		/* Make space for TEB */
		stack_top -= sizeof(struct y_thread_environment_block);
		teb = (void*)(ks->node.start + stack_top);
		
		/* 占用太多资源? */
		if (stack_top < 0)
			goto err1;
		
		ctx->stack_pos = ks->node.start + stack_top;
	}
	if (!ctx->stack0_size/* If caller give a specific stack(driver system will), we do not need to create */)
	{
		ctx->stack0_size	= KT_ARCH_THREAD_CP0_STACK_SIZE;
		ctx->stack0			= km_page_alloc_kerneled(ctx->stack0_size / PAGE_SIZE);
	}

	/* Has command line param? Caller will make sure the ctx->param is a valid kernel buffer */
	if (ctx->flags & KT_CREATE_STACK_AS_PARA)
	{
		/* Write the cmdline to kernel stack(底部) for user to fetch */
		if (strlen((char*)ctx->para) >= ctx->stack0_size)
			goto err1;
		strcpy((char*)ctx->stack0, (char*)ctx->para);

		/* Write the real thread param for this address, so we know where to fetch */
		ctx->para = (unsigned long)ctx->stack0;
	}
	
	/* Fate entry only can default if we are creating ring0 thread */
	if (!ctx->fate_entry)
	{
		if (ctx->cpl == KP_USER)
			goto err1;
		ctx->fate_entry	= (void*)kernel_thread_fate_entry;
	}

	/* Thread Object */
	p = cl_object_create(&thread_type);
	if (!p)	goto err1;
	kt_arch_init_thread(p, ctx);
	p->process	= where;
	p->teb		= teb;
	if (ctx->flags & KT_CREATE_RUN)
		kt_wakeup(p);
	
	return p;

err1:
	//TODO: To recall all resources
	
err0:
	return NULL;
}

void __noreturn kt_delete_current()
{
	//TODO
	TODO("");
	while (1) kt_schedule();
}

struct ko_thread *kt_create_kernel(void *entry, unsigned long para)
{
	struct kt_thread_creating_context ctx = {0};
	
	ctx.para			= para;
	ctx.thread_entry	= entry;
	ctx.flags			= KT_CREATE_RUN;
	return kt_create(kp_get_system(), &ctx);
}

struct ko_thread *kt_create_driver_thread(void *ring0_stack, int stack_size, void *entry, unsigned long para)
{
	struct kt_thread_creating_context ctx = {0};

	ctx.thread_entry	= entry;
	ctx.stack0			= ring0_stack;
	ctx.stack0_size		= stack_size;
	ctx.para			= para;
	return kt_create(kp_get_system(), &ctx);
}

/**
	@brief The recaller of the threads already dead.
 
	Should bind to specific CPU or the CPU operation while we are deleting thread/process will be error!!!
 */
void kt_run_killer()
{
}

bool kt_init()
{
	cl_object_type_register(&thread_type);
	init_idle_thread(kc_get_raw(), &init_thread);
	
	return true;
}

struct ko_thread *ke_current()
{
	return kt_arch_get_current();
}
