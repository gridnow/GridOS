/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程调度器
*/

#include <thread.h>
#include "process.h"
#include "cpu.h"
#include "kernel.h"

void kt_sleep(struct ko_thread *who, unsigned long stat)
{

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