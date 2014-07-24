/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *	 内核 完成体
 *   Wuxin
 */
#include <ddk/debug.h>

#include <kernel/ke_complete.h>

#include <thread.h>
#include <sync.h>

/*hal private */
#include "tick.h"

/**
	@brief Sleep milliseconds 

	休眠milliseconds并且不退出休眠直到时间流逝完
*/
void ke_msleep(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs) + 1;

	while (timeout)
		timeout = kt_timeout(kt_current(), timeout);
}

/**
	@brief Sleep in us unit

	由于US的单位比较小，线程不一定进入休眠状态，可能进入轮询状态
*/
void ke_usleep(unsigned int us)
{

}

/**
	@brief 等待一段时间，但是其间可以被唤醒

	@para[in] timeout 是超时时间，单位为tick
*/
signed long ke_schedule_timeout(signed long timeout)
{
	return kt_timeout(kt_current(), timeout);
}

/************************************************************************/
/* 完成体                                                                       */
/************************************************************************/

/**
	@brief Wait until timeout

	@return 0 on timeout or aborted
*/
static unsigned long __wait_common(struct ke_completion * x, unsigned long timeout)
{
	/* Enter critical section */
	ke_spin_lock(&x->lock);

	if (!x->done)
	{
		struct thread_wait cur = {0};

		/* Insert to the wait queue */
		cur.who = kt_current();
		list_add_tail(&cur.task_list, &x->task_list);

		/* Wait in loop until completion is done ... */
		ke_spin_unlock(&x->lock);
		while (!x->done)
		{	
			/* Just infinite wait? */
			if (timeout == KE_SYNC_WAIT_INFINITE)			
				kt_sleep(KT_STATE_WAITING_SYNC);
			else
			{
				timeout = ke_schedule_timeout(timeout);
				if (!timeout) break;
			}

			/* Thread may be killed */
			//TODO
		}
		ke_spin_lock(&x->lock);

		/* Remove from the wait queue */
		list_del(&cur.task_list);

		/* Still not done? */
		if (!x->done)
			goto timedout;
	}

	/* Restore the completion status */
	x->done--;
	ke_spin_unlock(&x->lock);

	/* 很有可能刚刚timeout了，但是条件发生了，因此怎么着也得给个1 */
	return timeout ?: 1;

timedout:
	ke_spin_unlock(&x->lock);
	return 0;
}

/**
	@brief 初始化一个完成体

	完成体要先被创立（一般是嵌在结构体中的），或者静态变量。
*/
void ke_init_completion(struct ke_completion * x)
{
	x->done = 0;
	INIT_LIST_HEAD(&x->task_list);
	ke_spin_init(&x->lock);
}

/**
	@brief 等待完成体处于就绪状态

	如果不完成，那么线一直程阻塞
*/
void ke_wait_for_completion(struct ke_completion * x)
{
	__wait_common(x, KE_SYNC_WAIT_INFINITE);
}

unsigned long ke_wait_for_completion_timeout(struct ke_completion * x, unsigned long timeout)
{
	return __wait_common(x, timeout);
}

/**
	@brief 完成了某个事情，唤醒第一个等待着
*/
void ke_complete(struct ke_completion * x)
{
	unsigned long flags;
	struct thread_wait * wait;

	/* Enter critical section */
	flags = ke_spin_lock_irqsave(&x->lock);

	/* 
		The done is increased even if nobody is waiting.
		And new wait will exist quickly because the counter is done! 
	*/
	x->done++;	

	/* Get the first thread to wakeup */
	if (!list_empty(&x->task_list))
	{
		wait = list_first_entry(&x->task_list, struct thread_wait, task_list);
		kt_wakeup(wait->who);
	}

	/* Leave critical section */
	ke_spin_unlock_irqrestore(&x->lock, flags);
}
