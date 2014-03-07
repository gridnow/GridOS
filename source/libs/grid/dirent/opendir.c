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
#include "sys/file_req.h"

#define DIR_BUFF_LEN   (4 * 1024)
#define dir_buff_malloc() crt_alloc(DIR_BUFF_LEN)

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
	dir->dir_buffer = dir_buff_malloc();
	if (NULL == dir->dir_buffer)
		goto err2;
	dir->total_size = DIR_BUFF_LEN;
	return dir;

err2:
	crt_free(dir);
err1:
	sys_close(dir_handle);
err:
	return NULL;
}

