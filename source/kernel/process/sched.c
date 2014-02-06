/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程调度器
*/

#include "thread.h"
#include "process.h"
#include "cpu.h"
#include <memory.h>

#include <bitops.h>
#include <hardirq.h>

static struct ko_thread *select_in_queue(struct kc_cpu *cpu)
{
	struct ko_thread *next;
	unsigned long bit;

#if 1 //sanity check:no thread? should never happen
	if (unlikely(!cpu->run_mask))
		ke_panic("no thread.");
#endif

	bit = __ffs(cpu->run_mask);	
	list_rotate_left(cpu->run_queue + bit);
	next = list_first_entry(cpu->run_queue + bit, struct ko_thread, queue_list);

#if 0
	/* Escape idle */
	if (next == cpu->idle)
	{	
		list_rotate_left(cpu->run_queue + bit);
		next = list_first_entry(cpu->run_queue + bit, struct ko_thread, queue_list);
		
	}
#endif

	return next;
}

static struct ko_thread *select_next(struct kc_cpu *cpu, struct ko_thread **prev)
{
	*prev = cpu->cur;
	return select_in_queue(cpu);
}

/**
	@brief 当前线程（被）放弃CPU使用权
 
	@note
		Interrupt must be disabled
 */
static void __schedule(int preempt)
{
	struct ko_thread *prev, *next, *last;
	struct kc_cpu *cpu;

	/* Acquiring current cpu, interrupt disabled so it's safe to use raw version */
	cpu = kc_get_raw();
	
	/* Select thread */
	next = select_next(cpu, &prev);
	if (unlikely(next == prev))
		goto exit;
	cpu->cur = next;
	
	/* Switch address space */
	if (next->process != prev->process)
	{
		km_arch_ctx_switch(&prev->process->mem_ctx, &next->process->mem_ctx);
	}
	
	/* Switch register */
	kt_arch_switch(prev, next);

exit:
	return;
}

/**
	@brief 切换线程
 
	@note
		1,保证当前线程是可被切换的（抢占控制计数器为0）
 */
void kt_schedule()
{
	unsigned long flags;
	
	raw_local_irq_save(flags);
	__schedule(0);
	raw_local_irq_restore(flags);

	return;
}

void kt_schedule_driver()
{
	unsigned long flags;

	struct ko_thread *prev, *next, *last;
	struct kc_cpu *cpu;

	raw_local_irq_save(flags);

	/* Acquiring current cpu, interrupt disabled so it's safe to use raw version */
	cpu = kc_get_raw();

	/* No driver thread at all, they are not inited */
	if (cpu->driver_thread == NULL)
		goto next;

	next = cpu->driver_thread;
	prev = cpu->cur;
	cpu->cur = next;

	/* Switch register */
	kt_arch_switch(prev, next);
next:
	raw_local_irq_restore(flags);

	return;
}

void kt_switch(int why)
{
	kt_schedule();
}

/**********************************************************************
	Scheduler Tick
**********************************************************************/
/* local timer tick */
void kt_sched_tick()
{
	unsigned long flags;

	if (hal_preempt_count() & PREEMPT_MASK)
		return;
	
	/* Tick handler MAY not close irq for safe preempt count check */
	local_irq_save(flags);
	
	/* Driver thread should no be preempted */
	if (kt_current()->desc_of_driver)
		goto next;
	
	if (!(hal_preempt_count() & PREEMPT_MASK))
			__schedule(1);
next:
	local_irq_restore(flags);
}

