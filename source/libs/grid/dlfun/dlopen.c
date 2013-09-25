/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#include <types.h>
#include <dlfcn.h>

#include "crt.h"

#include "dlfcn.h"

void *dlopen(const char *file, int mode)
{
	Dl_info * info = NULL;

	if (!file)
		return GLOBAL_HANDLE;

	info = crt_zalloc(sizeof(Dl_info));
	if (!info)
		goto dlopen_error;
	
	info->dli_fbase	= exefmt_load(file);
	info->dli_fname	= NULL;/*TODO: point to the image file name */
	if(!info->dli_fbase) goto dlopen_error;
	if(!info->dli_fname) goto dlopen_error;
	
	return info;

dlopen_error:
	if (info)
	{
		if (info->dli_fbase)
		{

		}

		crt_free(info);
	}

	return NULL;
}