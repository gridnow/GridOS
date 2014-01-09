/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#include <stdio.h>
#include <compiler.h>

#include "stream_file.h"

DLLEXPORT int fclose(FILE *file)
{
	struct file *filp = file_get_from_detail(file);

	return filp->ops->close(filp);
}