/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	The Grid Core Library
 
	Wuxin (82828068@qq.com)
*/

#include <compiler.h>
#include <stdio.h>

#include "cl_string.h"
#include "sys/ke_req.h"

#include "malloc/malloc.h"
#include "dlfun/dlfcn.h"
#include "stream_file.h"

DLLEXPORT FILE stdout =(FILE)1, stdin = (FILE)2, stderr = (FILE)3;

static bool init_libc()
{
	if (init_malloc() == false)
		goto err;
	if (init_module() == false)
		goto err;	
	if (stream_file_buffer_init() == false)
		goto err;
	
	return true;

err:
	return false;
}

void dll_preinit(void *dl_base)
{
	dl_handle_over(dl_base);
}

void dll_main()
{	
	if (init_libc() == false)
		goto err;
	
	return;

err:
	;
}

DLLEXPORT void set_errno(int value)
{

}

DLLEXPORT int get_errno()
{
	return 0;
}