/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */
#include <ddk/debug.h>

#include <stdio.h>
#include <compiler.h>

#include "file.h"

static int close_stream_file(struct stdio_file *file)
{
	return file->ops->close(file);
}

DLLEXPORT int fclose(FILE *file)
{
#if 0
	//TODO : stderr/stdout closing
	if (file == stderr || file == stdout)
		return 0;
	
	return close_stream_file((struct stdio_file * )file);
#endif
	TODO("");
}