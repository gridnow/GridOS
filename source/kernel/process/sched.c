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
#include "kernel.h"

#include <bitops.h>
#include <hardirq.h>

static struct ko_thread *select_in_queue(struct kc_cpu *cpu)
{
	struct ko_thread *next;
	unsigned long bit;

	/* 
		Rotate first!
		Select the running thread from the queue and the first node in queue is running.
		Because the idle thread is at first position on init.
		If only one thread in the queue, rotate left has no side side effect!
	*/
#if 1 //sanity check:no thread? should never happen
	if (unlikely(!cpu->run_mask))
		ke_panic("no thread.");
#endif
	bit = __ffs(cpu->run_mask);	
	list_rotate_left(cpu->run_queue + bit);
	next = list_first_entry(cpu->run_queue + bit, struct ko_thread, queue_list);
	
	return next;
}

static struct ko_thread *select_next(struct kc_cpu *cpu, struct ko_thread **prev)
{
	*prev = cpu->cur;
	return select_in_queue(cpu);
}

static void __schedule()
{
	struct ko_thread *prev, *next;
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
		//TODO:switch memory context
		printk("Switch memory context...\n");
	}
	//printk("next = %x\n ", next);
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
	__schedule();
	raw_local_irq_restore(flags);
	
	return;
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
	local_irq_save(flags);
	if (!(hal_preempt_count() & PREEMPT_MASK))
		__schedule();
	local_irq_restore(flags);
}

