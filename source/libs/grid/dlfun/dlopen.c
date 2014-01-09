/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#include <types.h>
#include <dlfcn.h>
#include <list.h>

#include "posix.h"

#include "dlfcn.h"

bool init_module()
{
	return true;
}

DLLEXPORT void *dlopen(const char *file, int mode)
{
	if (!file)
		return GLOBAL_HANDLE;

	return dl_open(file, mode);
}

DLLEXPORT void dlcall_posix_entry(void *entry, int argc, char *argv[])
{
	dl_call_posix_entry(entry, argc, argv);	
}
