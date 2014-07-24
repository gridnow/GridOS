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

DLLEXPORT void *dlentry(void *__restrict handle)
{
	return dl_entry(handle);
}

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
//		printf("dlsym û���ҵ����� %s.return addr %p.", name, __builtin_return_address(0));

	return p;
}

/**
	@brief ��һ���а汾��ŵķ��ŵ�ַ
	
	Find the run-time address in the shared object HANDLE refers to of the symbol called NAME with VERSION.  

	@param[in] handle ��̬�����
	@param[in] name ������
	@param[in] version �汾��
	@return 
		���ŵ�ַ��NULL����ʧ��
*/
void *dlvsym (void *__restrict handle,
					 __const char *__restrict name,
					 __const char *__restrict version)
{
	TODO("");
	return NULL;
}