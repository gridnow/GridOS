/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	The Grid Core Library
 
	Posix environment API
	llhhbc (lilonghui123456@163.com)
 */


#include <compiler.h>
#include <types.h>
#include "env.h"
#include "string.h"
#include "crt.h"
#include "malloc.h"
#include "stdio.h"

extern char *_environ ;		//环境变量首指针

DLLEXPORT __weak char *getenv(const char * name)
{
	char *p;
	int len;
	char flag;

	if (_environ == NULL)
	{
		return NULL; 	//for not null
	}

	flag = 0;

	LOCK_ENV_VARIABLE;

	for (p = _environ; *p; )
	{
		if (strcmp(p, name) == 0)
		{
			printf(" compare equ \n ");
			flag = 1;
		}
		p = p + strlen(name) + 1;
		memcpy(&len, p, sizeof(int));
		p += sizeof(int);
		if (flag)
		{
			printf(" find len is [%d]\n", len);
			UNLOCK_ENV_VARIABLE;
			return p;
		}
		p += len+1;	//eof
	}
	UNLOCK_ENV_VARIABLE;
	return NULL;
}
