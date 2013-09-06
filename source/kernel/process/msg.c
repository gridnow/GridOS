/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程异步消息
*/

#define DEBUG_OFFSET_START 1024
/************************************************************************/
/* 全局变量：系统中所有消息的缓冲区                                     */
/************************************************************************/
struct ke_message_pool
{
	void * pool;											
	struct cl_bitmap slots_bitmap;
	struct ke_spinlock lock;
};
static struct ke_message_pool ke_msg_pool;

/**
	@brief non-lock version of acquiring the msg node

	Used when the node is stable
*/
static struct ke_message * __get_pool(struct ko_thread * who)
{
	struct ke_message * message_node;
	message_node = &who->msg;
	return message_node;
}

/**
	@brief Acquire a message control block.

	If the thread dose not have the message control block, create one.
	If the thread has one, add the active counter to indicate it cannot be fade out.
*/
static struct ke_message * get_pool(struct ko_thread * who)
{
	struct ke_message * message_node;
	unsigned long r;

	message_node = &who->msg;
	
	ke_spin_lock(&message_node->lock);	
	if (message_node->desc.slots) 
		goto ok;
again:
	/* lock the global */
	ke_spin_lock(&ke_msg_pool.lock);

	/* really has no msg node, other people may alreay allocated */
	if (message_node->desc.slots)
		goto next;

	/* Get a slot buffer */
	r = hal_bitmap_alloc_bit(&ke_msg_pool.slots_bitmap);
	if (r == HAL_BITMAP_ALLOC_BIT_ERROR)
	{
		ke_spin_unlock(&ke_msg_pool.lock);
		kt_switch(KT_SWITCH_GIVE_CHANCE);
		goto again;
	}
	message_node->desc.slot_buffer_size = KE_MESSAGE_SLOTS_SIZE_PER_THREAD;
	message_node->desc.slots = message_node->desc.current_slot =
		ke_msg_pool.pool + r * KE_MESSAGE_SLOTS_SIZE_PER_THREAD;	
	
	/* Reset the all slots */
	message_reset_all(&message_node->desc);

	/* Leave critical section */
next:
	ke_spin_unlock(&ke_msg_pool.lock);
	
ok:
	message_node->activity++;
	ke_spin_unlock(&message_node->lock);	
	return message_node;
}

static void put_pool(struct ko_thread * who)
{
	struct ke_message * message_node;

	message_node = &who->msg;
	ke_spin_lock(&message_node->lock);	
	message_node->activity--;
	if (message_node->activity == 0)
	{
		/* IF the thread has no msg in the slots reclaim the slots buffer */
		hal_bitmap_dealloc_bit(&ke_msg_pool.slots_bitmap,
			((unsigned long)message_node->desc.slots - (unsigned long)ke_msg_pool.pool) / KE_MESSAGE_SLOTS_SIZE_PER_THREAD);

	}
	ke_spin_unlock(&message_node->lock);
}

/**
	@brief 发送消息
*/
static struct message * send(struct ko_thread * to, struct message * what, struct ke_message_wait * wait)
{
	struct ke_message * pool = get_pool(to);
	struct message * dst, * tmp, * src;
	int count, i;

	count = what->count;

#if DEBUG_OFFSET_START
	//debug
	if (*(unsigned char*)(ke_msg_pool.pool - 1) != 0x87)
		TRACE_ERROR("头溢出");
	//debug
	if (*(unsigned char*)(ke_msg_pool.pool + KE_MESSAGE_SLOTS_SIZE_PER_THREAD) != 0x87)
		TRACE_ERROR("尾溢出");
#endif

	/* 
		从dst开始分配连续的slot, 分配成功后更新dst。
		1, find
		2, set
	*/
	ke_spin_lock(&pool->lock);
	dst = pool->desc.current_slot;
	tmp = dst;
	i = 0;
	do
	{
		if (MSG_STATUS_USED_BIT & dst->flags)								//Not found, pool is full
			break;
		i++;																//找到一个
		MSG_GET_NEXT_SLOT(&pool->desc, dst);		
	} while (dst != tmp && i < count + 1/*the head*/);					//A cycle was found or the desired count
	if (i != count + 1/*the head*/) 
	{
		ke_spin_unlock(&pool->lock);
		printk("The message pool is full, count is %d.\n", count);
		goto full;
	}

	dst = tmp;
	for (i = 0; i < count + 1/*the head*/; i++)
	{
		dst->flags |= MSG_STATUS_USED_BIT;
		MSG_GET_NEXT_SLOT(&pool->desc, dst);			
	}
	pool->desc.current_slot = dst;
	ke_spin_unlock(&pool->lock);

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
		MSG_GET_NEXT_SLOT(&pool->desc, dst);

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
		wait->who_sent = kt_get_current();

		/* Add wait to the thread msg waiting list */
		ke_spin_lock(&pool->lock);
		list_add_tail(&wait->list, &to->msg.waiting);
		ke_spin_unlock(&pool->lock);
	}

	/* 
		Fire... 
	*/
	dst = tmp;
	dst->count	= what->count;
	dst->what	= what->what;
	dst->flags |= what->flags;												//FLAGS must a the end
	dst->flags |= MSG_STATUS_NEW;											//The New will cause dst thread to read the msg
	put_pool(to);
	return dst;

full:	
	put_pool(to);
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
bool ktm_send(struct ko_thread * to, struct message * what)
{
	struct ke_message_wait wait;
	struct message *sent;
	bool r = false;

try_again:
	if (what->flags & MSG_FLAGS_SYNC)	
		sent = send(to, what, &wait);
	else
		sent = send(to, what, NULL);
	if (sent == NULL)
		goto resend;
	
	/* 唤醒接收者 */
	kt_wakeup(to);	

	/* Wait the result if is sync from "sent" */
	if (what->flags & MSG_FLAGS_SYNC)
	{
		/* 
			Handled? Waiting... 
		*/
		if (KT_WAIT(wait.wait_status != KE_MESSAGE_WAIT_STATUS_NONE) != KT_WAIT_OK)
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
			struct message * dst, * src;
			int response_count = wait.sent->count;
			int request_count  = what->count;
			struct ke_message *src_pool = get_pool(to);

			/*
				The write back count cannot exceeds the size when we was requesting
			*/
			if (response_count > request_count)
			{
				response_count = request_count;
				hal_printf_warning("同步消息的回应尺寸不可大于发送时的尺寸，裁掉。\n");
			}

			/*
				Copy body
			*/
			src = wait.sent;
			dst = what;
			for (i = 0; i < response_count; i++)
			{
				/* Escape head */
				MSG_GET_NEXT_SLOT(&src_pool->desc, src);
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
			
			/* Release src message pool */
			put_pool(to);

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
	
	/* 如果消息根本那太大，放不下，那么你重试也是放不下，此时要判断是不是太大了，否则GIVE_CHANCE后还是这样 */
	//TODO

	/*
		Give a chance for receiver to response the pool.
		And check the dst status: if it's killing.
		And check the src(me) status : if it's killing.
	*/
	kt_switch(KT_SWITCH_GIVE_CHANCE);
	if (kt_thread_status(to) == KT_THREAD_STATUS_KILLING ||
		kt_thread_status(kt_get_current()) == KT_THREAD_STATUS_KILLING)
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
struct ke_message * ktm_prepare_loop(struct ko_thread * who)
{
	struct ke_message * pool = get_pool(who);
	return pool;
}

/**
	@brief Ack the sync message

	The responser acknowledges a synchronous message
*/
void ktm_ack_sync(struct ko_thread * who, struct message * what)
{
	struct list_head * pos;
	struct ke_message_wait * wait = NULL;
	struct ke_message * pool = __get_pool(who);

	/* Release the wait node */
	ke_spin_lock(&pool->lock);	
	list_for_each(pos, &who->msg.waiting)
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
	ke_spin_unlock(&pool->lock);

	if (!wait)
		hal_panic("回应同步消息时为什么没有找到等待节点？");	

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
	
	/* Receiver(This time is myself) will clean the used bit when ACKED this sync msg after return from here */
}

/**
	@brief Cancel all msg of a thread(responser) and delete the pool

	Normally responser is bean killing abruptly
*/
void ktm_delete(struct ko_thread * who)
{
	struct list_head * pos;
	struct ke_message_wait * wait = NULL;
	struct ke_message * pool = get_pool(who);

	ke_spin_lock(&pool->lock);	
again:
	list_for_each(pos, &who->msg.waiting)
	{
		wait = list_entry(pos, struct ke_message_wait, list);		
		list_del(pos);

		/* set status wakeup the sender to read it */
		wait->wait_status = KE_MESSAGE_WAIT_STATUS_ABANDONED;		
		kt_wakeup(wait->who_sent);

		goto again;
	}
	ke_spin_unlock(&pool->lock);
	
	put_pool(who);
}

/**
	@brief Init the message subsystem
*/
bool ktm_init()
{
	bool r = false;
	int count;

	KE_SPINLOCK_INIT(&ke_msg_pool.lock);

	/* The message buffer */
	ke_msg_pool.pool = kmm_page_alloc_kerneled(KE_MESSAGE_SLOTS_BUFFER_SIZE - DEBUG_OFFSET_START);	
	if (!ke_msg_pool.pool)
		goto end;
	//DEBUG 
	memset(ke_msg_pool.pool, 0x87, KE_MESSAGE_SLOTS_BUFFER_SIZE);
	ke_msg_pool.pool += DEBUG_OFFSET_START;
	
	/* The allocator */
	r = hal_bitmap_adaption(&ke_msg_pool.slots_bitmap, ke_msg_pool.pool,
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
void ktm_msg_init(struct ko_thread * who)
{
	KE_SPINLOCK_INIT(&who->msg.lock);
	INIT_LIST_HEAD(&who->msg.waiting);
}