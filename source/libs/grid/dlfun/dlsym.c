/**
	The Grid Core Library
*/

/**
	Posix shared object operations
	Yaosihai
*/

#include <types.h>
#include <dlfcn.h>
#include <errno.h>
#include <stdio.h>
#include <ddk/debug.h>

#include "posix.h"
#include "dlfcn.h"

DLLEXPORT void *dlsym(void *__restrict handle, __const char *__restrict name)
{
	void *p = NULL;
	
	if (handle == GLOBAL_HANDLE)
	{		
		set_errno(ENOSYS);
		goto dlsym_end;
	}

	p = dl_sym(handle, name);
	
dlsym_end:	
//	if(!p)
//		printf("dlsym 没有找到符号 %s.return addr %p.", name, __builtin_return_address(0));

	return p;
}

/**
	@brief 获一个有版本编号的符号地址
	
	Find the run-time address in the shared object HANDLE refers to of the symbol called NAME with VERSION.  

	@param[in] handle 动态库对象
	@param[in] name 符号名
	@param[in] version 版本号
	@return 
		符号地址，NULL则是失败
*/
void *dlvsym (void *__restrict handle,
					 __const char *__restrict name,
					 __const char *__restrict version)
{
	TODO("");
	return NULL;
}