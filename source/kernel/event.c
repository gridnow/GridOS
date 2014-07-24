/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *	 Event
 *   Wuxin
 */

#include <kernel/ke_event.h>
#include <kernel/ke_memory.h>

#include <sync.h>
#include <thread.h> 

#include "object.h"

static kt_sync_status satisfied(struct kt_sync_base *sync, struct ko_thread * thread)
{
	struct ke_event * event = (struct ke_event *)sync;
	
	/* If is auto reset,we reset now */
	if(!event->manual_reset)
		event->status = KE_SYNC_SIGNALED_NO;
	return KE_SYNC_SATISFY_OK;
}

static kt_sync_status signaled(struct kt_sync_base *sync, struct ko_thread * thread)
{
	struct ke_event * event =(struct ke_event*)sync;
	return event->status;
}

static struct kt_sync_ops sync_ops = {
	.satisfied	= satisfied,
	.signaled	= signaled,
};

static int set_event(struct ke_event * p)
{
	unsigned int count = 1;

	KE_SYNC_OBJ_LOCK(p);
	p->status = KE_SYNC_SIGNALED_OK;

	/* Wake up all waiters if manual reset, a single one otherwise */
	if (p->manual_reset) count = 0;
	count = kt_sync_wakeup(TO_SYNC(p), count);

	KE_SYNC_OBJ_UNLOCK(p);
	return count;
}

static void reset_event(struct ke_event * p)
{
	KE_SYNC_OBJ_LOCK(p);
	p->status = KE_SYNC_SIGNALED_NO;
	KE_SYNC_OBJ_UNLOCK(p);
}

void ke_event_init(struct ke_event *event, bool manual_reset, bool initial_status)
{
	kt_sync_init(TO_SYNC(event), &sync_ops);
	if(initial_status)
		event->status			= KE_SYNC_SIGNALED_OK;
	else
		event->status			= KE_SYNC_SIGNALED_NO;
	event->initial				= initial_status;
	event->manual_reset			= manual_reset;
}

struct ke_event *ke_event_object_create(bool manual_reset, bool initial_status)
{
	struct ke_event *event;
	void *tmp_addr = km_valloc(sizeof(*event) + sizeof(struct cl_object));
	if (!tmp_addr)
		return NULL;
	
	/* We have to construct an object for user handle to use */
	event = tmp_addr + sizeof(struct cl_object);
	ke_event_init(event, manual_reset, initial_status);
	
	return event;
}

int ke_event_set(struct ke_event *event)
{
	return set_event(event);
}

void ke_event_reset(struct ke_event *event)
{
	reset_event(event);
}

kt_sync_wait_result ke_event_wait(struct ke_event * event, unsigned int timeout)
{
	return kt_wait_object(kt_current(), TO_SYNC(event), timeout);
}

kt_sync_wait_result ke_events_wait(int count, struct ke_event * events[], bool wait_all, unsigned int timeout, int *id)
{
	return kt_wait_objects(kt_current(), count, TO_SYNCS(events), wait_all, timeout, id);
}