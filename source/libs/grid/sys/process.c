/**
	The Grid Core Library
 */

/**
	Reuqest kernel for process
 */
#include <types.h>
#include <ystd.h>

#include "sys/ke_req.h"

DLLEXPORT y_handle y_process_create(xstring name, char *cmdline)
{
	struct sysreq_process_create req;

	req.base.req_id = SYS_REQ_KERNEL_PROCESS_CREATE;
	req.name		= name;
	req.cmdline		= cmdline;

	return (y_handle)system_call(&req);
}
