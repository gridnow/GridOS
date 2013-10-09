/**
	The Grid Core Library
*/

#include <compiler.h>
#include <stdio.h>

#include "cl_string.h"
#include "sys/ke_req.h"

#include "malloc/malloc.h"

static bool init_libc()
{
	bool r;
	printf("%s %s %d.\n", __FILE__, __FUNCTION__, __LINE__);
	r = init_malloc();
	printf("%s %s %d.\n", __FILE__, __FUNCTION__, __LINE__);
	if (r == false)
		goto err;

	return true;

err:
	return false;
}

void dll_main()
{
	printf("%s %s %d.\n", __FILE__, __FUNCTION__, __LINE__);
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