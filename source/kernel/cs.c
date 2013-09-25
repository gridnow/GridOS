/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *	 内核 临界区
 *   Wuxin
 */
#include <kernel/ke_critical.h>
#include <string.h>

#include "spinlock.h"

DLLEXPORT void ke_critical_init(struct ke_critical_section * cs)
{
	memset(cs, 0, sizeof(*cs));
	ke_spin_init(&cs->lock);
	ke_init_completion(&cs->complete);
}

DLLEXPORT void ke_critical_enter(struct ke_critical_section * cs)
{
	/* Close the scheduler */
	//ADD CODE HERE

#ifdef CONFIG_SMP
	unsigned long times = cs->deal_count;
	do 
	{
		/* LOCK success? */
		if (arch_spin_trylock((arch_spinlock_t*)&cs->lock)) 
		{
			/* 上次是休眠过了，那么我们应该调整增加SPIN COUNT */
			if (times == 0)
			{
				cs->deal_count++;
			}
			/* 是直接抢到的，那么我们应该减少spin count */
			else
			{
				cs->deal_count--;
			}
			break;
		}
		
		/* Used out the times? We will sleep */
		if (times == 0)
		{
			ke_wait_for_completion(&cs->complete);
		}
		else
			times--;
	} while (1);
#endif
}

DLLEXPORT void ke_critical_leave(struct ke_critical_section * cs)
{
	/* Open scheduler */
#ifdef CONFIG_SMP
	arch_spin_unlock((arch_spinlock_t*)&cs->lock);
	ke_complete(&cs->complete);
#endif
}