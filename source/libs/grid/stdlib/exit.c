/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	The Grid Core Library
 
	Posix environment API
	Wuxin (82828068@qq.com)
 */
#include <compiler.h>
#include "sys/ke_req.h"

DLLEXPORT void exit(int status)
{
	struct sysreq_process_startup st;

	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_END;								// End of process
	st.ret_code			= status;
	system_call(&st);

	while(1);
}

