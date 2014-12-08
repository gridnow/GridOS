/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#include <stdio.h>
#include <dlfcn.h>
#include <errno.h>

#include "posix.h"
#include "dlfcn.h"

DLLEXPORT int dladdr(void * address, Dl_info * info)
{
	TODO("");
	set_errno(ENOSYS);
	return 0;
}

DLLEXPORT void *dlsection_vaddr(void *handle, const char *section_name, size_t *size)
{
	if (handle == GLOBAL_HANDLE)
		return NULL;
	
	return dl_section_vaddress(handle, section_name, size);
}