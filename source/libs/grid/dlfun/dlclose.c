/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#include <types.h>
#include <dlfcn.h>
#include <errno.h>

#include "posix.h"

#include "dlfcn.h"

DLLEXPORT int dlclose(void *handle)
{
	if (handle == GLOBAL_HANDLE)
		return POSIX_FAILED;
	
	dl_close(handle);
	
	return POSIX_SUCCESS;
}