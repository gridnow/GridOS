/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程调度器
*/

#include <thread.h>
#include <sync.h>
#include "process.h"
#include "cpu.h"

bool kt_sleep(unsigned long stat)
{
	unsigned long flags;
	bool suspend = true;
	struct ko_thread *who = kt_current();
	
	spin_lock_irqsave(&who->ops_lock, flags);
	if ((who->state & KT_STATE_MASK) == KT_STATE_RUNNING)
	{
		struct kc_cpu * cpu;
		
		/*
			But have wakeup count?
			This may happen when the thread want to sleep, before another wakeup is comming.
		*/
		if (!(stat & KT_STATE_ADD_FORCE_BY_SYSTEM))
		{
			if (who->wakeup_count != 0)
			{
				who->wakeup_count = 0;
				suspend = false;
				goto end;
			}
		}
		else
			who->state |= KT_STATE_ADD_FORCE_BY_SYSTEM;
		
		/* Really suspend */
		cpu = kc_get_raw();
		list_del_init(&who->queue_list);
		cpu->run_count[who->priority_level]--;
		cpu->running_count--;
		if (cpu->run_count[who->priority_level] == 0)
			__clear_bit(who->priority_level, &cpu->run_mask);
		
		suspend = true;

	}
	who->state &= ~KT_STATE_MASK;
	who->state |= stat & KT_STATE_MASK;
	
end:
	spin_unlock_irqrestore(&who->ops_lock, flags);
	
	/* Now the cpu is still at hand, just switch if needed */
	//TODO: 但是可能刚刚判断对了要切换，定时器就来切换了，那么此时切换了2才次，多切换了一次。
	if (!(stat & KT_STATE_ADD_NO_SWITCHING) && suspend)
		kt_schedule();
	return suspend;
}

void kt_wakeup_driver(struct ko_thread *who)
{
	struct kc_cpu * cpu;
	unsigned long flags;
	
	//Sanity check
#if 1
	if (who->desc_of_driver == NULL)
		ke_panic("唤醒驱动的线程不是驱动线程。");
	if (!list_empty(&who->queue_list))
		ke_panic("唤醒驱动线程时必须是无队列状态");
#endif
	
	spin_lock_irqsave(&who->ops_lock, flags);
	
	cpu = kc_get_raw();
	cpu->driver_thread = who;
//	cpu->running_count++;
	
	/* Set as running status */
	who->state &= ~KT_STATE_MASK;
	who->state |= KT_STATE_RUNNING;
	
	spin_unlock_irqrestore(&who->ops_lock, flags);
}

void kt_wakeup(struct ko_thread *who)
{
	unsigned long flags;
	
	spin_lock_irqsave(&who->ops_lock, flags);
	if ((who->state & KT_STATE_MASK) != KT_STATE_RUNNING)
	{
		struct kc_cpu * cpu;
		
		/* Normal wakeup cannot ops "FORCE by system " */
		if (KT_STATE_ADD_FORCE_BY_SYSTEM & who->state)
			goto end;
		//sanity check
#if 1
		/* Dead thread? */
		if (KT_STATE_ADD_DIEING & who->state)
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
		
		/* 
			Insert the thread at the head, 
			so we can make sure newly woken up driver thread to be selected immediately.
		*/
		list_add_tail(&who->queue_list, cpu->run_queue + who->priority_level);
//		list_add(&who->queue_list, cpu->run_queue + who->priority_level);
		__set_bit(who->priority_level, &cpu->run_mask);
		cpu->run_count[who->priority_level]++;
		cpu->running_count++;
		
		/* Set as running status */
		who->state &= ~KT_STATE_MASK;
		who->state |= KT_STATE_RUNNING;
	}
	else
		who->wakeup_count ++;

end:
	spin_unlock_irqrestore(&who->ops_lock, flags);
}


/***********************************************************/
/* Export */
/***********************************************************/
signed long ke_sleep_timeout(signed long timeout)
{
	while (timeout)
		timeout = kt_timeout(NULL, timeout);
	return timeout;
}
