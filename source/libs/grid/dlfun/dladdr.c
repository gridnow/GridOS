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

#include <ddk/debug.h>

#include "posix.h"
#include "dlfcn.h"

int dladdr(void * address, Dl_info * info)
{
	TODO("");
	set_errno(ENOSYS);
	return 0;
}
