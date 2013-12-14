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

DLLEXPORT size_t fwrite(void *ptr, size_t size, size_t nmemb, FILE *file)
{
	printf("%s", ptr);
}
