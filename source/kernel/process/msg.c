/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   �߳��첽��Ϣ
*/
#include <kernel/ke_memory.h>

#include <process.h>
#include <memory.h>
#include <thread.h>
#include <sync.h>
#include <section.h>

#include <cl_string.h>
#include <ddk/debug.h>
#define DEBUG_OFFSET_START 0

/************************************************************************/
/* ȫ�ֱ�����ϵͳ��������Ϣ�Ļ�����                                     */
/************************************************************************/
struct ke_message_pool
{
	void  *pool;
	struct ko_section *ks_message;
	struct cl_bitmap   slots_bitmap;
	struct ke_spinlock lock;
};
static struct ke_message_pool ke_msg_pool;

static void put_thread_msg(struct ko_thread *who)
{
	struct ktm *msg = &who->msg;
	
	struct list_head *pos, *n;
	struct ke_message_wait *wait;
	void *slots_base;
	
	/*
		Somebody may be sending something to us...
		So we have to keep atomic.
	 */
	slots_base = msg->desc.slots;
	
	ke_spin_lock(&msg->lock);
	list_for_each_safe(pos, n, &msg->waiting)
	{
		wait = list_entry(pos, struct ke_message_wait, list);
		list_del(&wait->list);
		
		/* Wakeup the sender to read it */
		wait->wait_status = KE_MESSAGE_WAIT_STATUS_ABANDONED;
		kt_wakeup(wait->who_sent);
	}
	msg->desc.slots = msg->desc.current_slot = NULL;
	ke_spin_unlock(&msg->lock);
	
	if (msg->map)
	{
		ks_close(KT_GET_KP(who), msg->map);
		msg->map = NULL;
	}
	
	ke_spin_lock(&ke_msg_pool.lock);
	cl_bitmap_dealloc_bit(&ke_msg_pool.slots_bitmap,
						  ((unsigned long)slots_base - (unsigned long)ke_msg_pool.pool) / KE_MESSAGE_SLOTS_SIZE_PER_THREAD);
	ke_spin_unlock(&ke_msg_pool.lock);
}

static struct ktm *get_thread_msg(struct ko_thread *who, bool try_create)
{
	struct ktm *msg = &who->msg;
	
	/*	
		�����ű�֤��ǰ�̱߳�����who��Ҳ���Ǵ��������ٸö�������Ǹ��̡߳�
		���򣬴�Ҷ���Ϊĳһ���̴߳��������ٻᵼ���߼�����
	*/
	if (msg->map == NULL && try_create == true)
	{
		int offset;
		unsigned long r;
		
		BUG_ON(who != kt_current());
		
		if ((msg->map = ks_create(KP_CURRENT(), KS_TYPE_SHARE, 0, PAGE_SIZE, KM_PROT_READ | KM_PROT_WRITE)) == NULL)
			goto err;
		
		ke_spin_lock(&ke_msg_pool.lock);
		r = cl_bitmap_alloc_bit(&ke_msg_pool.slots_bitmap);
		ke_spin_unlock(&ke_msg_pool.lock);
		if (r == HAL_BITMAP_ALLOC_BIT_ERROR)
		{
			/*
			 TODO: STOLEN.
			 kt_switch(KT_SWITCH_GIVE_CHANCE);
			 */
			goto err1;
		}
		msg->desc.slot_buffer_size = KE_MESSAGE_SLOTS_SIZE_PER_THREAD;
		msg->desc.slots = msg->desc.current_slot =
			ke_msg_pool.pool + r * KE_MESSAGE_SLOTS_SIZE_PER_THREAD;
		message_reset_all(&msg->desc);
		
		offset = (unsigned long)msg->desc.slots - (unsigned long)ke_msg_pool.pool;
		ks_share(kp_get_system(), ke_msg_pool.ks_message,
				 KT_GET_KP(who), msg->map,
				 offset);
	}
	
	return msg;
err1:
	ks_close(KP_CURRENT(), msg->map);
	msg->map = NULL;
err:
	return NULL;
}

/**
	@brief ������Ϣ
*/
static struct y_message *send(struct ktm *where, struct y_message *what, struct ke_message_wait *wait)
{
	struct y_message *dst, *tmp, *src;
	int count, i;

	count	= what->count;
	i		= 0;

	/* 
		�����Ϣ�ش��ڵĻ���
		��dst��ʼ����������slot, ����ɹ������dst��
		1, find
		2, set
	*/
	ke_spin_lock(&where->lock);
	dst = where->desc.current_slot;
	if (!dst)
	{
		ke_spin_unlock(&where->lock);
		goto full;
	}
	tmp	= dst;
	do
	{
		if (MSG_STATUS_USED_BIT & dst->flags)								//Not found, pool is full
			break;
		i++;																//�ҵ�һ��
		MSG_GET_NEXT_SLOT(&where->desc, dst);
	} while (dst != tmp && i < count + 1/*the head*/);						//A cycle was found or the desired count
	if (i != count + 1/*the head*/) 
	{
		ke_spin_unlock(&where->lock);
		//printk("The message pool is full, count is %d.\n", count);
		goto full;
	}

	dst = tmp;
	for (i = 0; i < count + 1/*the head*/; i++)
	{
		dst->flags |= MSG_STATUS_USED_BIT;
		MSG_GET_NEXT_SLOT(&where->desc, dst);
	}
	where->desc.current_slot = dst;

	/* 
		Set the data from the end to start.
		The first node is the real msg node and checked by receiver.
		The data copying may use a lot of cpu cycle and can be placed outside the spinlock.
	*/
	dst = tmp;
	src = what;
	for (i = 0; i < count; i++)
	{
		/* Escape the head */
		src++;
		MSG_GET_NEXT_SLOT(&where->desc, dst);

		/* Copy node */
		*dst = *src;	
	}

	/*
		Set the wait info before firing the message, the receiver may finish the message
		immediately after the firing, but wait info has not been setup.
	*/
	if (wait)
	{
		wait->sent = tmp;
		wait->wait_status = KE_MESSAGE_WAIT_STATUS_NONE;
		wait->who_sent = kt_current();

		/* Add wait to the thread msg waiting list */
		list_add_tail(&wait->list, &where->waiting);
	}
	ke_spin_unlock(&where->lock);
	
	/* 
		Fire... 
	*/
	dst = tmp;
	dst->count	= what->count;
	dst->what	= what->what;
	dst->flags |= what->flags;												//FLAGS must a the end
	dst->flags |= MSG_STATUS_NEW;											//The New will cause dst thread to read the msg
	return dst;

full:
	return NULL;
}
/************************************************************************/
/*                                                                      */
/************************************************************************/

/**
	@brief The message for a thread

	@note
		Message can almost always be sent out successfully.
		But if the destination is in abnormal status(such as killing by force) it will return false.
		If I am in abnormal status, the message sending will fail too.
*/
bool ktm_send(struct ko_thread *to, struct y_message *what)
{
	struct ke_message_wait wait;
	struct y_message *sent;
	bool r = false;
	int try_resend = 3;
	struct ktm *where;
	
	where = get_thread_msg(to, false);
	
try_again:
	if (what->flags & MSG_FLAGS_SYNC)	
		sent = send(where, what, &wait);
	else
		sent = send(where, what, NULL);
	if (sent == NULL)
		goto resend;
	
	kt_wakeup(to);

	/* Wait the result if is sync from "sent" */
	if (what->flags & MSG_FLAGS_SYNC)
	{
		/* 
			Handled? Waiting... 
			���ﲻ����ֵȴ��ͳ������������
			1������Լ���ɱ��KT_WAIT ��ʶ��
			2��������˱�ɱ��
				1�������KT_WAIT ֮ǰ��ɱ����ô��wait_status ΪKE_MESSAGE_WAIT_STATUS_ABANDONED��
				2�������KT_WAIT �м䱻ɱ����ôKT_WAIT ���ᱻ���߳ɹ�����Ϊɱ���߽���������һ����1 �Ļ��Ѽ�������
				3�������KT_WAIT ֮��ɱ����ô��ɱ���ߴ�����״̬�л��ѡ�
		*/
		if (KT_WAIT(wait.wait_status != KE_MESSAGE_WAIT_STATUS_NONE) != KE_WAIT_OK)
		{
			printk("Message waiter met !KT_WAIT_OK.\n");
		}

		/*
			Now read the status.
			Write back the response if available or the message is abandoned.
		*/
		if (wait.wait_status == KE_MESSAGE_WAIT_STATUS_OK)
		{
			int i;
			struct y_message *dst, *src;
			int response_count = wait.sent->count;
			int request_count  = what->count;

			//TODO: check if we can write to user space directly;
			
			/*
				The write back count cannot exceeds the size when we was requesting
			*/
			if (response_count > request_count)
			{
				response_count = request_count;
				hal_printf_warning("ͬ����Ϣ�Ļ�Ӧ�ߴ粻�ɴ��ڷ���ʱ�ĳߴ磬�õ���\n");
			}

			/*
				Copy body
			*/
			src = wait.sent;
			dst = what;
			for (i = 0; i < response_count; i++)
			{
				/* Escape head */
				MSG_GET_NEXT_SLOT(&where->desc, src);
				dst++;

				/* Copy body */
				*dst = *src;
			}

			/* 
				Copy head 
			*/
			dst = what;
			src = wait.sent;
			*dst = *src;

			/* 
				Tell the receiver we have finished the copying process
			*/
			wait.sent->flags &= ~MSG_STATUS_NEW;
			r = true;
		}
		else
		{
			printk("The message is abandoned.\n");
		}
	}
	else
		r = true;
end:
	return r;
resend:
	
	/* �����Ϣ������̫�󣬷Ų��£���ô������Ҳ�ǷŲ��£���ʱҪ�ж��ǲ���̫���ˣ�����GIVE_CHANCE�������� */
	if (--try_resend == 0)
		goto end;

	/*
		Give a chance for receiver to response the pool.
		And check the dst status: if it's killing.
		And check the src(me) status : if it's killing.
	*/
	kt_switch(KT_SWITCH_GIVE_CHANCE);
	if (KT_CURRENT_KILLING() ||
		KT_THREAD_KILLING(to) == KT_STATE_KILLING)
	{
		r = false;
		goto end;
	}
	goto try_again;
}

/**
	@brief The thread want to get the slots to wait message

	In real wait, the pool may be released!
*/
struct ktm *ktm_prepare_loop()
{
	return get_thread_msg(kt_current(), true);
}

/**
	@brief Ack the sync message

	The responser acknowledges a synchronous message
*/
void ktm_ack_sync(struct y_message *what)
{
	struct list_head *pos;
	struct ke_message_wait *wait = NULL;
	struct ktm *msg;
	struct ko_thread *who = kt_current();
	
	msg = get_thread_msg(who, false);

	/* 
		Release the wait node.
		TODO: ��û�б�Ҫ�ø����ٵ÷����ҵ��ȴ��ߡ�
	*/
	ke_spin_lock(&msg->lock);
	list_for_each(pos, &msg->waiting)
	{
		wait = list_entry(pos, struct ke_message_wait, list);
		if (wait->sent == what)
		{
			list_del(&wait->list);
			break;
		}
		else
			wait = NULL;
	}
	ke_spin_unlock(&msg->lock);

	/* ����msg ����������,���û�б����٣������������٣���ôwait ���ȶ��á� */
	if (!wait)
		goto end;
	/*
		Wakeup the sender so it can copy back the result.
	*/	
	wait->wait_status = KE_MESSAGE_WAIT_STATUS_OK;
	kt_wakeup(wait->who_sent);

	/* 
		If the NEW MESSAGE is not set, the sender has finished copying result.
		Then the receiver then is safe to clean this message, including the USED bit.
		So the message slot can be used again.
	*/
	while (what->flags & MSG_STATUS_NEW)
	{
		/* Let the receiver have a chance to get the result */
		kt_switch(KT_SWITCH_GIVE_CHANCE);
	}
end:;
	/* Receiver(This time is myself) will clean the used bit when ACKED this sync msg after return from here */
}

/**
	@brief Cancel all msg of a thread(responser) and delete the pool

	Normally responser is bean killing abruptly
*/
void ktm_delete()
{
	struct ko_thread *who;
	
	/*
		ɾ����Ϣ����������Լ��̣߳��ʹ���һ������
	*/
	who = kt_current();
	put_thread_msg(who);
}

/*
	@brief �ṩ���ں˱�̽ӿ�ʹ��
*/
bool ke_msg_send(struct ko_thread *to, struct y_message *what)
{
	return ktm_send(to, what);
}


/**
	@brief Init the message subsystem
*/
bool ktm_init()
{
	bool r = false;

	ke_spin_init(&ke_msg_pool.lock);

	/* The message buffer */
	if (NULL == km_alloc_virtual(KE_MESSAGE_SLOTS_BUFFER_SIZE, KM_PROT_READ | KM_PROT_WRITE, (void**)&ke_msg_pool.ks_message))
		goto end;
	ke_msg_pool.pool = (void*)ke_msg_pool.ks_message->node.start;
	
	//DEBUG 
	memset(ke_msg_pool.pool, 0x87, KE_MESSAGE_SLOTS_BUFFER_SIZE);
	ke_msg_pool.pool += DEBUG_OFFSET_START;
	
	/* The allocator */
	r = cl_bitmap_adaption(&ke_msg_pool.slots_bitmap, ke_msg_pool.pool,
		KE_MESSAGE_SLOTS_BUFFER_SIZE - DEBUG_OFFSET_START,
		KE_MESSAGE_SLOTS_SIZE_PER_THREAD);
	if (r == false)
		goto end;
end:
	return r;
}

/**
	@brief Init the thread message node

	called at the thread creating
*/
void ktm_msg_init(struct ko_thread *who)
{
	memset(&who->msg, 0, sizeof(who->msg));
	INIT_LIST_HEAD(&who->msg.waiting);
	ke_spin_init(&who->msg.lock);
}