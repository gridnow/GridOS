/**
	The Grid Core Library
*/

#include <compiler.h>
#include <stdio.h>
#include <string.h>


#include "sys/ke_req.h"

#include "malloc/malloc.h"

/* ÃüÁîÐÐ´«µÝ */
static char cmdline_buffer[SYSREQ_PROCESS_STARTUP_MAX_SIZE] = {' '};
static char * exe_split_point = "·Ö¸î¿ÕÏ¶";

static bool init_libc()
{
	bool r;
	
	r = init_malloc();
	if (r == false)
		goto err;

	return true;

err:
	return false;
}

static void load_app()
{
	struct sysreq_process_startup st;
	char *exe_name;

	/* Get the cmd line */
	st.base.req_id = SYS_REQ_KERNEL_PROCESS_STARTUP;
	st.main_function = (unsigned long)load_app;
	st.cmdline_buffer = cmdline_buffer;
	system_call(&st);
	exe_name = strchr(cmdline_buffer, ' ');
	if (!exe_name)
		exe_name = cmdline_buffer;
	else
	{
		*exe_name = 0;
		exe_split_point = exe_name;
		exe_name = cmdline_buffer;
	}
}

void dll_main()
{
	if (init_libc() == false)
	{
		goto err;
	}

	load_app();
	
	return;

err:
	;
}

DLLEXPORT void set_errno(int value)
{

}

