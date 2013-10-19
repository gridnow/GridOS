/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#include <stdio.h>
#include <compiler.h>

#include "file.h"

static size_t read_stream_file(void *ptr, ssize_t size, ssize_t nmemb, struct stdio_file *file)
{
	int i, ret;
	size_t items = 0;
	
	for (i = 0; i < nmemb; i++)
	{
		ret = file->ops->read(file, ptr, size);
		if (0 > ret)
			break;
		else if (0 == ret)
		{
			items = 0;
		}
		else
		{
			items++;
			ptr = (unsigned char*)ptr + size;
		}
	}
	
	return items;
}

DLLEXPORT size_t fread(void *ptr, size_t size, size_t nmemb, FILE *file)
{
	return read_stream_file(ptr, size, nmemb, (struct stdio_file *)file);
}