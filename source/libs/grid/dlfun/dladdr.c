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

int dladdr(void * address, Dl_info * info)
{
	TODO("");
	set_errno(ENOSYS);
	return 0;
}
