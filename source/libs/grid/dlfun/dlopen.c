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

#if 0
#ifndef __mips__
/* Set lazy linker */
elf_set_lazy_linker(&obj->exe_desc, so_lazy_link, &obj->user_ctx);
#endif
#endif

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