/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ͬ�����������
*/
#include <debug.h>

#include <sync.h>
#include <thread.h>

#include <ddk/compatible.h>

static void wakeup_timeout(unsigned long para)
{
	struct ko_thread * who = (struct ko_thread*)para;
	kt_wakeup(who);
}

void kt_sync_init(struct kt_sync_base * sync, struct kt_sync_ops * ops)
{
	sync->ops = ops;
	INIT_LIST_HEAD(&sync->wait_queue);
	spin_lock_init(&sync->lock);
}

/*
	����ĳ��ͬ�������ϵ�COUNT���ȴ���
 
	@note
		1,count ȡֵΪ 0 Ϊ���еĶ���,�������ǻ���Ҫ��ĸ���
		2,�����߱�֤�����Ǳ���ס��.
 
	@return The count of threads that have been woke up
 */
int kt_sync_wakeup(struct kt_sync_base * sync, int count)
{
	int i = 0;
	struct list_head * list, * n;
	struct thread_wait * who;
	
	/*Loop all count to wakeup the waiters*/
	list_for_each_safe(list, n, &sync->wait_queue)
	{
		who = list_entry(list, struct thread_wait, task_list);
		kt_wakeup(who->who);
		
		if (count && !--count) break;
		i ++;
	}
	
	return i;
}

kt_sync_wait_result kt_wait_objects(struct ko_thread * who, int count, struct kt_sync_base ** objects, bool wait_all, unsigned int timeout, int *id)
{
	int i;
	
	kt_sync_wait_result status = KE_WAIT_ABANDONED;
	bool waiter_added			= false;
	struct thread_wait			static_wait_node[KE_SYNC_STATIC_WAIT_NODE_COUNT];
	struct thread_wait *		waited_node[Y_SYNC_MAX_OBJS_COUNT] = {0};
	int translated_timeout		= 0;
	
	/* Some method we do not support now */
	if (wait_all == true)
	{
		TODO("�ȴ��¼�ʱҪ�������¼�ȫ�������ų�������ʱû��֧�֡�\n");
		goto out3;
	}
	if (count > Y_SYNC_MAX_OBJS_COUNT)
		goto out3;
	
check_satisfied:
	for (i = 0; i < count; i++)
		KE_SYNC_OBJ_LOCK(objects[i]);
	
	for (i = 0; i < count; i++)																/*The object is triggered now?*/
	{
		kt_sync_status sync_status;
		
		sync_status = objects[i]->ops->signaled(objects[i], who);							/*��û�б�����?*/
		if (sync_status == KE_SYNC_SIGNALED_OK)
		{
			status = KE_WAIT_OK;
			if (id)
				*id = i;
			objects[i]->ops->satisfied(objects[i] ,who);									/*���߶���,���ǵȵ���*/
			goto out2;
		}
		else if (sync_status == KE_SYNC_SIGNALED_CANCEL)
		{
			status = KE_WAIT_ABANDONED;
			goto out2;
		}
	}
	
	/*
		û���źţ��Ƿ�Ҫ�ȴ�ʱ�������
	 */
	if (timeout == 0)
	{
		status = KE_WAIT_TIMEDOUT;
		goto out2;
	}
	
	/*
		Wait the object changing,if no change happened ,we goto sleep.
	*/
	if (waiter_added == false)
	{
		for (i = 0; i < count; i++)																/*add to waiter list and begin to wait*/
		{
			if (i < KE_SYNC_STATIC_WAIT_NODE_COUNT)
			{
				waited_node[i] = &static_wait_node[i];
				waited_node[i]->who = who;
				list_add_tail(&waited_node[i]->task_list, &objects[i]->wait_queue);
			}
			else
			{
				TRACE_UNIMPLEMENTED("��ʱ����ȴ��ڵ�");
				waited_node[i] = NULL;
			}
			if (NULL == waited_node[i])
			{
				TRACE_ERROR("�ȴ��������ʱ�޷�����ȴ��ڵ�");
				status = KE_WAIT_ABANDONED;
				goto out2;
			}
		}
		waiter_added = true;
	}
	for (i = 0; i < count; i++)
		KE_SYNC_OBJ_UNLOCK(objects[i]);															/* Unlock object to give other people a chance to change the object */
	if (translated_timeout == 0 && timeout == KE_SYNC_WAIT_INFINITE)								/* The timeout is original */
	{
		kt_sleep(KT_STATE_WAITING_SYNC);														/* ���ޣ��ȴ��¼��ķ��� */
	}
	else
	{
		/* Translate timeout to tick */
		if (!translated_timeout)
		{
			timeout = hal_ms_to_tick(timeout) + 1;
			translated_timeout = 1;
		}
		timeout = kt_timeout(who, timeout);
	}

	/* �߳��ѹ����ˣ����¼������ˣ� ���������ˣ�*/
	if (KT_CURRENT_KILLING())
	{
		status = KE_WAIT_ABANDONED;
		goto out1;
	}

	/* �ã���ô����ȥ����¼�״̬ */
	goto check_satisfied;
	
out1:
	for (i = 0; i < count; i++)																	/*Lock all object for unlock,shit ...*/
		KE_SYNC_OBJ_LOCK(objects[i]);
	
out2:
	for (i = 0; i < count; i++)
	{
		if (waited_node[i] != NULL)
		{
			list_del(&waited_node[i]->task_list);
			if (i >= KE_SYNC_STATIC_WAIT_NODE_COUNT)
			{
				TRACE_UNIMPLEMENTED("�ͷŷ���ĵȴ��ڵ�");
				break;
			}
		}
	}
	for (i = 0; i < count; i++)
		KE_SYNC_OBJ_UNLOCK(objects[i]);
	
out3:
	return status;
}

kt_sync_wait_result kt_wait_object(struct ko_thread * who, struct kt_sync_base *p, unsigned int timeout)
{
	return kt_wait_objects(who, 1, &p, false, timeout, NULL);
}

/**
	@brief Wait timeout
 */
signed long kt_timeout(struct ko_thread * who, signed long timeout)
{
	struct timer_list wait_timer = {0};
	unsigned long expire;
	
	expire = timeout + hal_get_tick();
	if (!who) who = kt_current();
	
	/* Add to timer */
	hal_timer_init(&wait_timer, 0, NULL);
	wait_timer.function = wakeup_timeout;
	wait_timer.expires	= expire;
	wait_timer.data		= (unsigned long)who;
	hal_timer_add(&wait_timer);
	
	/* Wait by sleep */
	kt_sleep(0);
	
	/* Delete the timer */
	hal_timer_del(&wait_timer);
	
	timeout = expire - hal_get_tick();

	return timeout < 0 ? 0 : timeout;
}
