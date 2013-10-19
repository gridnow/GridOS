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
#include "file.h"

static bool init_libc()
{
	if (init_malloc() == false)
		goto err;
	if (stream_file_buffer_init() == false)
		goto err;

	return true;

err:
	return false;
}

void dll_main()
{
//	printf("%s %s %d.\n", __FILE__, __FUNCTION__, __LINE__);
	if (init_libc() == false)
		goto err;
	
	return;

err:
	;
}

DLLEXPORT void set_errno(int value)
{

}

/************test**************/
__weak void exefmt_load()
{
	
}

__weak void printk()
{
	
}