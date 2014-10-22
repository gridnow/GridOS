/**
	The Grid Core Library
 */

/**
	Reuqest kernel for process
 */
#include <types.h>
#include <stdio.h>
#include <ystd.h>

#include "crt.h"
#include "sys/ke_req.h"
#include "common/include/message.h"

/* We are operating on the file exclusively */
static LIST_HEAD(registered_handler);
#define LOCK_REGISTERED_HANDLER()  //TODO
#define UNLOCK_REGISTERED_HANDLER()

struct message_desc
{
	struct list_head list;
	message_id_t msg_id;
	y_message_func callback;
};

static struct y_message_instance *get_current_mi()
{
	struct y_thread_environment_block *current = get_current();
	return current->mi;
}

static void set_current_mi(struct y_message_instance *mi)
{
	struct y_thread_environment_block *current = get_current();
	current->mi = mi;
}

static void message_default_sync_ack(struct y_message *what)
{
	struct sysreq_thread_msg req;
	
	req.base.req_id = SYS_REQ_KERNEL_THREAD_MSG;
	req.ops			= SYSREQ_THREAD_MSG_ACK_SYNC;
	req.send.msg	= what;
	system_call(&req);
}

static void message_default_wait(struct y_message_instance *message_instance)
{
	struct sysreq_thread_msg req;
	
	req.base.req_id = SYS_REQ_KERNEL_THREAD_MSG;
	req.ops			= SYSREQ_THREAD_MSG_SLEEP;
	system_call(&req);
	
	message_instance->slots				= req.slot_base;
	message_instance->slot_buffer_size	= req.slot_buffer_size;
}

static y_message_func message_default_find_handler(struct y_message_instance *message_instance, message_id_t id)
{
	struct message_desc *desc;
	
	LOCK_REGISTERED_HANDLER();
	list_for_each_entry(desc, &registered_handler, list)
	{
		if (desc->msg_id == id)
		{
			UNLOCK_REGISTERED_HANDLER();
			return desc->callback;
		}
	}
	
	UNLOCK_REGISTERED_HANDLER();
	return NULL;
}

DLLEXPORT y_handle y_process_create(xstring name, char *cmdline)
{
	struct sysreq_process_create req;

	req.base.req_id = SYS_REQ_KERNEL_PROCESS_CREATE;
	req.name		= name;
	req.cmdline		= cmdline;

	return (y_handle)system_call(&req);
}

DLLEXPORT y_wait_result y_process_wait_exit(y_handle for_who, unsigned long * __in __out result)
{
	TODO("");
	return KE_WAIT_ERROR;
}

DLLEXPORT y_msg_loop_result y_message_loop()
{
	struct y_message_instance message_instance;
	
	message_instance.current_slot	= NULL;
	message_instance.filter			= NULL;
	message_instance.sleep			= message_default_wait;
	message_instance.response_sync	= message_default_sync_ack;
	message_instance.find_handler	= message_default_find_handler;

	/* Set current mi for user to use in some message API */
	set_current_mi(&message_instance);
	message_loop(&message_instance);
	
	return Y_MSG_LOOP_EXIT_SIGNAL;		
}

DLLEXPORT bool y_message_register(message_id_t message_id, y_message_func call_back_func)
{
	struct message_desc *node;
	
	/* Already registered before? */
	if (NULL != message_default_find_handler(get_current_mi(), message_id))
		goto err;
	
	/* Allocate a new one */
	if (NULL == (node = crt_alloc(sizeof(*node))))
		goto err;
	node->callback	= call_back_func;
	node->msg_id	= message_id;
	
	LOCK_REGISTERED_HANDLER();
	list_add_tail(&node->list, &registered_handler);
	UNLOCK_REGISTERED_HANDLER();
	return true;
	
err:
	return false;
}

DLLEXPORT bool y_message_send(ke_handle to_thread, struct y_message *what)
{
	struct sysreq_thread_msg req;
	
	req.base.req_id		= SYS_REQ_KERNEL_THREAD_MSG;
	req.ops				= SYSREQ_THREAD_MSG_SEND;
	req.send.to_thread	= to_thread;
	req.send.msg		= what;
	return system_call(&req);
}

DLLEXPORT int y_message_writeback(struct y_message *what, int wb_count, ...)
{
	int i, count;
	va_list wb_list;
	
	MSG_PDATA_TYPE cur;
	MSG_DATA_TYPE data;
	
	struct y_message_instance *mi;

	mi = get_current_mi();
	count = what->count;
	if (wb_count > count)
		return -1;
	
	va_start(wb_list, wb_count);
	MSG_DATA_START(mi, what, cur);
	
	for (i = 0; i < wb_count; i++)
	{
		data = va_arg(wb_list, MSG_DATA_TYPE);
		MSG_DATA_WRITE_NEXT(mi, cur, data, MSG_DATA_TYPE);
	}
	
	return i;
}

DLLEXPORT int y_message_read(struct y_message *what, ...)
{
	int i, count;
	va_list wb_list;

	MSG_PDATA_TYPE cur, wb;
	MSG_DATA_TYPE data;
	
	struct y_message_instance *mi;
	
	mi = get_current_mi();
	count = what->count;
	va_start(wb_list, what);
	MSG_DATA_START(mi, what, cur);

	for (i = 0; i < count; i++)
	{
		MSG_DATA_READ_NEXT(mi, cur, data, MSG_DATA_TYPE);
		wb = va_arg(wb_list, MSG_PDATA_TYPE);
		*wb = data;
	}

	return i;
}

DLLEXPORT y_wait_result y_wait_objects(y_handle * __user sync_objects, int count, int timeout)
{
	struct sysreq_thread_sync req;
	
	req.base.req_id = SYS_REQ_KERNEL_SYNC;
	req.ops			= SYSREQ_THREAD_SYNC_WAIT_OBJS;
	req.detail.wait_objs.sync_objects = sync_objects;
	req.detail.wait_objs.count = count;
	req.detail.wait_objs.timeout = timeout;
	
	return (y_wait_result)system_call(&req);
}

DLLEXPORT y_wait_result y_event_wait(y_handle event, int timeout)
{
	y_handle sync_object[] = { event };
	return y_wait_objects(sync_object, 1, timeout);
}

DLLEXPORT int y_event_set(y_handle event)
{	
	struct sysreq_thread_sync req;
	
	req.base.req_id = SYS_REQ_KERNEL_SYNC;
	req.ops			= SYSREQ_THREAD_SYNC_EVENT;
	req.detail.event.ops = 's';
	req.detail.event.event = event;
	
	return (int)system_call(&req);
}

DLLEXPORT y_handle y_event_create(bool manual_reset, bool initial_status)
{
	struct sysreq_thread_sync req;
	
	req.base.req_id = SYS_REQ_KERNEL_SYNC;
	req.ops			= SYSREQ_THREAD_SYNC_EVENT;
	req.detail.event.ops       = 'c';
	req.detail.event.is_manual = manual_reset;
	req.detail.event.is_set    = initial_status;
	return (y_handle)system_call(&req);
}

DLLEXPORT void y_event_delete(y_handle event)
{
	struct sysreq_thread_sync req;
	
	req.base.req_id = SYS_REQ_KERNEL_SYNC;
	req.ops			= SYSREQ_THREAD_SYNC_EVENT;
	req.detail.event.event = event;
	req.detail.event.ops = 'd';
	
	system_call(&req);
}
