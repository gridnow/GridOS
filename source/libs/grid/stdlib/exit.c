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
#include <unistd.h>

#include "sys/ke_req.h"

DLLEXPORT void exit(int status)
{
	struct sysreq_process_startup st;

	/* TODO: ll functions registered with atexit(3) and on_exit(3) are called, in the reverse order of their registration.*/
	
	/* 
		All open stdio(3) streams are flushed and closed.  Files created by tmpfile(3) are removed.
		The exit() function causes normal process termination and the value of status & 0377 is returned to the parent (see wait(2)).
	*/	
	st.base.req_id		= SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.func				= SYSREQ_PROCESS_STARTUP_FUNC_END;								// End of process
	st.ret_code			= status;
	system_call(&st);

	while(1);
}

