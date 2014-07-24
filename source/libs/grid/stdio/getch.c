/**
 The Grid Core Library
 */

/**
 Posix file
 ZhaoYu,Yaosihai
 */

#include <stdio.h>
#include <compiler.h>

DLLEXPORT int getch()
{
	unsigned long key = 0;

	int ret = stdin_read(&key);
	if (ret < 0)
		return ret;
	//printf("getch %d, %d\n", ret, key);
	return key;
}