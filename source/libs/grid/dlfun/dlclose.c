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

#include "crt.h"
#include "posix.h"

#include "dlfcn.h"

int dlclose(void * handle)
{
	Dl_info * info = handle;

	if (info == GLOBAL_HANDLE)
		return POSIX_FAILED;
	
	if (info->dli_fbase)
		;//TODO

	crt_free(info);

	return POSIX_SUCCESS;
}