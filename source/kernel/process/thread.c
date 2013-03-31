/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程管理
*/

#include "thread.h"
#include "memory.h"
#include "object.h"
#include "process.h"
#include "cpu.h"
#include "sched.h"
#include "kernel.h"

#include <string.h>

static struct ko_thread *init_thread;

#define ka_call_dynamic_module_entry(f, p) f(p)
static asmregparm void kernel_thread_fate_entry(unsigned long (*thread_entry)(unsigned long),unsigned long para)
{
	if (thread_entry)
		ka_call_dynamic_module_entry(thread_entry, para);
	//TODO delete the kernel thread
}

static bool object_close(real_object_t *obj)
{

}

static void object_init(real_object_t *obj)
{
	struct ko_thread *thread = (struct ko_thread *)obj;
	INIT_LIST_HEAD(&thread->queue_list);
	ke_spin_init(&thread->ops_lock);
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
	cl_object_type_register(&thread_type);
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
struct ko_thread * kt_create(struct ko_process * where, void * wrapper, void * func, unsigned long param, unsigned long flags)
{
	struct kt_thread_creating_context ctx = {0};
	struct ko_thread * p;

	/* Create the thread from micro layer */
	ctx.thread_entry 	= (unsigned long)func;
	ctx.para		 	= param;	
	if (where->cpl == KP_USER)
	{		
		ctx.fate_entry = (unsigned long)wrapper;
		ctx.stack_pos	 = 0;
		//TODO create stack
	}
	else
	{
		if (!wrapper) wrapper = kernel_thread_fate_entry;
		ctx.fate_entry	= (unsigned long)wrapper;
		ctx.stack0		= 0;
		ctx.stack0_size	= 4096;

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
	p->process = where;
	
	return p;

err1:
	
err0:
	return NULL;
}

void kt_wakeup(struct ko_thread *who)
{
	unsigned long flags;

	flags = ke_spin_lock_irqsave(&who->ops_lock);										//No ops on who, and no IRQ
	if (!test_bit(KT_STATE_RUNNING, &who->state))
	{
		struct kc_cpu * cpu;

		/* Normal wakeup cannot ops "FORCE by system " */
		if (test_bit(KT_STATE_ADD_FORCE_BY_SYSTEM, &who->state))
			goto end;
		//sanity check
#if 1
		/* Dead thread? */
		if (test_bit(KT_STATE_ADD_DIEING, &who->state))
		{
			ke_panic("内核唤醒一个死亡的线程.");
		}

		/* In other queue? Error! */
		if (!list_empty(&who->queue_list))
		{
			ke_panic("内核唤醒一个已经在某个队列中的线程."); 			
		}
#endif
		/* Move thread to its running queue */
		cpu = kc_get_raw();
		list_add_tail(&who->queue_list, cpu->run_queue + who->priority_level);			
		__set_bit(who->priority_level, &cpu->run_mask);
		cpu->run_count[who->priority_level]++;
		cpu->running_count++;

		/* Set as running status */
		__set_bit(KT_STATE_RUNNING, &who->state);
	}
	else
		who->wakeup_count ++;
end:
	ke_spin_unlock_irqrestore(&who->ops_lock, flags);
}

bool kt_init()
{
	init_idle_thread(kc_get_raw(), &init_thread);
	return true;
}
