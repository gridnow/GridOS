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

DLLEXPORT size_t fread(void *ptr, size_t size, size_t nmemb, FILE *file)
{
	int i, ret;
	size_t items = 0;
	struct file *filp = file_get_from_detail(file);
	
	for (i = 0; i < nmemb; i++)
	{
		ret = filp->ops->read(filp, ptr, size);
		if (0 >= ret)
			break;		
		else
		{
			items++;
			ptr = (unsigned char*)ptr + size;
		}
	}
	
	return items;
}