/**
	The Grid Core Library
 */

/**
	Reuqest kernel for process
 */
#include <types.h>
#include <stdio.h>
#include <ystd.h>

#include "sys/ke_req.h"
#include "common/include/message.h"

static struct y_thread_environment_block *get_current()
{
	struct y_thread_environment_block *current;
	struct sysreq_thread_teb req;
	
	req.base.req_id = SYS_REQ_KERNEL_THREAD_TEB;
	current = (struct y_thread_environment_block*)system_call(&req);
	
	return current;
}

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

static void default_sync_message_ack()
{
	//TODO
}

static void default_message_wait(struct y_message_instance *message_instance)
{
	struct sysreq_thread_msg req;
	
	req.base.req_id = SYS_REQ_KERNEL_THREAD_MSG;
	system_call(&req);
	
	message_instance->slots				= req.slot_base;
	message_instance->slot_buffer_size	= req.slot_buffer_size;
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
	return Y_SYNC_WAIT_ERROR;
}

DLLEXPORT int y_thread_wait_event()
{
	TODO("");
	return 0;
}

DLLEXPORT y_msg_loop_result y_message_loop()
{
	struct y_message_instance message_instance;
	
	message_instance.current_slot	= NULL;
	message_instance.filter			= NULL;
	message_instance.sleep			= default_message_wait;
	message_instance.response_sync	= default_sync_message_ack;

	/* Set current mi for user to use in some message API */
	set_current_mi(&message_instance);
	message_loop(&message_instance);
	
	return Y_MSG_LOOP_EXIT_SIGNAL;		
}

DLLEXPORT bool y_message_send(struct y_message *what)
{
	TODO("");
	return false;	
}

DLLEXPORT void y_message_read(struct y_message *what, ...)
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

}