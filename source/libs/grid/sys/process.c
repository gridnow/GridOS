/**
	The Grid Core Library
 */

/**
	Reuqest kernel for process
 */
#include <types.h>
#include <ystd.h>

#include "sys/ke_req.h"
#include "common/include/message.h"

static void default_sync_message_ack()
{
	//TODO
}

static void default_message_wait(struct message_instance *message_instance)
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

DLLEXPORT int y_thread_wait_event()
{
	return 0;
}

DLLEXPORT y_msg_loop_result y_message_loop()
{
	struct message_instance message_instance;
	struct sysreq_thread_msg req;

	message_instance.current_slot	= NULL;
	message_instance.filter			= NULL;
	message_instance.sleep			= default_message_wait;
	message_instance.response_sync	= default_sync_message_ack;

	message_loop(&message_instance);
	
	return Y_MSG_LOOP_EXIT_SIGNAL;
	
err:
	return Y_MSG_LOOP_ERROR;
	
}

DLLEXPORT bool y_message_send(struct message *what)
{
	
}

