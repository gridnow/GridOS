/**
	The Grid Core Library
*/

/**
	Posix directory API
	Yaosihai,ZhaoYu
*/
#include <compiler.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "posix.h"
#include "dir.h"
#include "file.h"

DLLEXPORT DIR *opendir(const char *name)
{
	DIR	*dir;
	ke_handle dir_handle;

	dir_handle = dir_open(name);
	if (KE_INVALID_HANDLE== dir_handle)
		goto err;	

	/* ´´½¨DIR */
	dir = (DIR *)crt_zalloc(sizeof(*dir));
	if (NULL == dir)
		goto err1;
	dir->dir_handle	= dir_handle;

	return dir;
	
err1:
	sys_close(dir_handle);
err:
	return NULL;
}

