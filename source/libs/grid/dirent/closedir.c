/**
	The Grid Core Library
*/

/**
	Posix directory API
	Yaosihai,ZhaoYu
*/

#include <compiler.h>
#include <dirent.h>

#include "posix.h"
#include "file.h"
#include "dir.h"

DLLEXPORT int closedir (DIR *dirp)
{
	sys_close(dirp->dir_handle);
	crt_free(dirp);
	return 0;
}
