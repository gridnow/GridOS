/**
	The Grid Core Library
 */

/**
	Message slots
 */

#include <types.h>
#include "message.h"

/**
	@brief ����������Ϣ��״̬��ʼ��
 
	@return
		The next slots after count
 */
static struct message *message_clean(struct message_instance *instance, struct message *start, int count)
{
	struct message * cur = start;
	
	while(count > 0)
	{
		cur->flags = 0;
		MSG_GET_NEXT_SLOT(instance, cur);
		count --;
	}
	return cur;
}

/**
	@brief ������Ϣ
 
	�����Ϣ��ͬ���ģ���Ҫ�����ں�ȥ��Ӧ����Ϊ�������ڵȴ����ǵĻ�Ӧ��
	�����Ϣ���첽�ģ���ֱ�����ռ��λ���ɡ�
 
	@return
		The next postion to fetch the message.The message length is dynmaic!
 */
static struct message *handle_message(struct message_instance *instance, struct message *what)
{
	int count;
	int r = true;
	struct message * next;
	
	if (instance->filter) r = instance->filter(what);
	count = what->count;										//record the src count before handling message, sync msg will overwrite it.
	
	if (r == true)
	{
		/*
		 1����ַ�����͵ģ�
		 2����ô���ǲ�ѯ��
		 */
		if (what->flags & MSG_FLAGS_ADDRESS)
		{
			void (*fn)(struct message * what);
			fn = (void (*)())what->what;
			fn(what);
		}
		
		//TODO : LOOKUP
	}
	
	/*
		ACK the sender if it's a synchronouse message
	 */
	if (what->flags & MSG_FLAGS_SYNC)
	{
		/*
		 The sender is waiting in kernel,
		 so let kernel know the message has bean handled and
		 let the sender copy back the result.
		 When these are all done, we can safely clean the message.
		 */
		instance->response_sync(what);
	}
	
	/*
		Clear the message.
		The head is cleaned when the data part has been cleaned.
	 */
	next = what;
	MSG_GET_NEXT_SLOT(instance, next);
	next = message_clean(instance, next, count);
	what->flags = 0;
	
	return next;
}

/**
	@brief ��������Ϣ��״̬��ʼ��
 */
void message_reset_all(struct message_instance *instance)
{
	struct message *cur = instance->slots;
	
	while(1)
	{
		cur->flags = 0;
		MSG_GET_NEXT_SLOT(instance, cur);
		if (cur == instance->slots)
			break;
	}
}

/**
	@brief �ȴ�һ����Ϣ��������
 */
void message_loop(struct message_instance *instance)
{
	struct message *cur;

	instance->sleep(instance);
	cur = instance->slots;

	while(1)
	{
		/*
			Check for first loop
		 */
		while (cur->flags & MSG_STATUS_NEW)
			cur = handle_message(instance, cur);

		/*
			Ŀǰ,���û����Ϣ,���ߵ�һ�ο��ܲ��ɹ�,��Ϊ������wakeup����������ô����Ҫȥ�������
			���,�������ϻص�������CUR��״̬,���Ϊ�ս������ǵڶ�������һ���ǳɹ���.
		 */
		instance->sleep(instance);
	}
}
