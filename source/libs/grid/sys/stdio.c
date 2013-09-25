/**
	The Grid Core Library
*/

/**
	Reuqest kernel for session IO
	Yaosihai
*/
#include <types.h>
#include <errno.h>

#include "sys/ke_req.h"
#include "std_console.h"

/**
	@brief Read a key
*/
int stdin_read(unsigned long * key)
{
	return -ENOSYS;
}

int stdout_write(unsigned char * buf, size_t size)
{
	struct sysreq_process_printf req = {0};
	req.base.req_id = SYS_REQ_KERNEL_PRINTF;
	req.string = buf;
	system_call(&req);
}
