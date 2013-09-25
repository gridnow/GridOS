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

#include "dlfcn.h"

void *dlsym(void *__restrict handle, __const char *__restrict name)
{
	void *p = NULL;
	Dl_info * info = handle;

	if (info == GLOBAL_HANDLE)
	{		
		set_errno(ENOSYS);
		goto dlsym_end;
	}

	/* TODO: to find symbol */
	
	
dlsym_end:	
	if(!p)	
		printf("dlsym û���ҵ����� %s.return addr %h.", name, __builtin_return_address(0));

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