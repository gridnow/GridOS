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
#include "sys/session_req.h"
#include "std_console.h"

/**
	@brief Read a key
*/
int stdin_read(unsigned long *key)
{
	struct sysreq_session_stdio req;
	int ret;
	
	/* Call the session to read */
	req.base.req_id = SYS_REQ_SESSION_STDIO_READ;
	ret = system_call(&req);
	if (ret < 0) return ret;
	
	/* Get the key */
	//*key = req.contents.input.pkg.code;
	return ret;
}

int stdout_write(char *buf, size_t size)
{
	struct sysreq_process_printf req;
	req.base.req_id = SYS_REQ_KERNEL_PRINTF;
	req.string = buf;
	return system_call(&req);
}
