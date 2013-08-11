/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *	 mutex
 *   Wuxin
 */

#include <kernel/ke_mutex.h>

#include <sync.h>
#include <thread.h> 

static kt_sync_status satisfied(struct kt_sync_base *sync, struct ko_thread * thread)
{
	struct ke_mutex * mutex = (struct ke_mutex *)sync;
	
	if (!mutex->count++)  /* FIXME: avoid wrap-around */
		mutex->owner = thread;
	
	return KE_SYNC_SATISFY_OK;
}

static kt_sync_status signaled(struct kt_sync_base *sync, struct ko_thread * thread)
{
	int r = KE_SYNC_SIGNALED_NO;
	struct ke_mutex * mutex = (struct ke_mutex *)sync;
	
	if((!mutex->count || (mutex->owner == thread)))
		r = KE_SYNC_SIGNALED_OK;
	return r;
}

/* Release a mutex once the recursion count is 0, return the number of thread wokenup */
static int do_release(struct ke_mutex * mutex)
{
	mutex->owner = NULL;
	return kt_sync_wakeup(TO_SYNC(mutex), 0);
}

static struct kt_sync_ops sync_ops = {
	.satisfied	= satisfied,
	.signaled	= signaled,
};

void ke_mutex_init(struct ke_mutex *mutex, bool initial_ownership)
{
	kt_sync_init(TO_SYNC(mutex), &sync_ops);
	
	/* The mutex is locked(owned by current thread) if argue for ownership */
	if (initial_ownership)
		satisfied(TO_SYNC(mutex), kt_current());
}

bool ke_mutex_lock(struct ke_mutex *mutex)
{
	/* Wait the object */
	if (kt_wait_object(kt_current(), TO_SYNC(mutex), Y_SYNC_WAIT_INFINITE) != KE_WAIT_OK)
		return false;
	
	return true;
}

unsigned long ke_mutex_unlock(struct ke_mutex *mutex)
{
	unsigned long pre_count;
	int wokenup = 0;
	/* Enter critical section */
	KE_SYNC_OBJ_LOCK(mutex);
	
	/* We are owning the mutex? */
	pre_count = mutex->count;
	if (!pre_count || (mutex->owner != kt_current()))
		goto end;
	
	/* »Ø¹ö¼ÆÊýÆ÷ */
	if (!--mutex->count)
		wokenup = do_release(mutex);
	
	/* Leave crictical section */
end:
	KE_SYNC_OBJ_UNLOCK(mutex);
	
	/* Give other people a chance to run, to prevent current thread to gain mutex again while other is waiting */
	if (wokenup)
	{
		kt_schedule();
	}
	
	return pre_count;
}
