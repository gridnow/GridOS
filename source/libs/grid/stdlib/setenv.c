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
#include <errno.h>
#include "env.h"

char *_environ = NULL;			//环境变量首指针
char *_cur_env = NULL;			//指向环境变量空闲空间首地址
unsigned long _env_len = NULL;	//当前环境变量指针所指向的空间的总大小

DLLEXPORT __weak  int setenv(const char * name, const char * value, int overwrite)
{
	char *p = _environ, *tmpp;
	int oldlen, newlen, tmplen;
	if (name == NULL || *name == '\0' || strchr (name, '=') != NULL)
	{
		set_errno(EINVAL);
		return EINVAL;
	}

	LOCK_ENV_VARIABLE;
	if (p == NULL)
	{
		_environ = (char*)malloc(__INITSIZE_);
		_env_len = __INITSIZE_;
		memset(_environ, 0x00, _env_len);
		_cur_env = _environ;		
	}
	UNLOCK_ENV_VARIABLE;

	newlen = strlen(name) + strlen(value) + sizeof(int) + __SIZE_OF_END_CHAR_ + __SIZE_OF_END_CHAR_;
	if ((_cur_env - _environ) + newlen  > _env_len)
	{
		/* 暂不关心重新分配空间情况 */
		return ENOMEM;
	}

	LOCK_ENV_VARIABLE;
	/* 查询是否已经存在 */
	for (p = _environ; *p ; )   
	{
		if (strcmp(p, name) == 0)
		{
			break;
		}
		p = p + strlen(name) + __SIZE_OF_END_CHAR_;	// name eof
		memcpy(&oldlen, p, sizeof(int));
		p += sizeof(int);
		p += oldlen + __SIZE_OF_END_CHAR_;			//eof
	}

	;
	if (*p == 0)			//需新增
	{
		p = _cur_env;
		strcpy(p, name);
		p += strlen(name);
		*p = NULL;		//eof
		p++;  
		newlen = strlen(value);
		memcpy(p, (char*)&newlen, sizeof(int));
		p += sizeof(int);
		strcpy(p, value);

		p = p + strlen(value);
		*p = 0;
		p++;
		_cur_env = p;
	}
	else if (overwrite == 1)
	{
		printf(" exists, overwrite it [%s] ",p);
		tmpp = p;
		oldlen = strlen(name);
		oldlen++;
		p += oldlen;
		memcpy((char*)&tmplen, p, sizeof(int));
		p += sizeof(int);
		oldlen += tmplen + sizeof(int) + __SIZE_OF_END_CHAR_;

		if (oldlen >= newlen)				//值长度变短，内存复用，不用偏移
		{
			printf(" 复用 \n ");
			memset(p, 0x00, tmplen+__SIZE_OF_END_CHAR_);
			strcpy(p, value);
		}
		else
		{
			p += tmplen + __SIZE_OF_END_CHAR_;
			memcpy(tmpp, p, _cur_env-p); 		//将原来的空间删除
			
			p = _cur_env - oldlen;			//加在最后面
			memset(p, 0x00, oldlen);
			strcpy(p, name);
			p += strlen(name);
			*p = 0;					//eof
			p++;  
			newlen = strlen(value);
			memcpy(p, (char*)&newlen, sizeof(int));
			p += sizeof(int);
			strcpy(p, value);

			p += strlen(value);
			*p = 0;
			p++;
			_cur_env = p;
		}
	}
	else
	{
		UNLOCK_ENV_VARIABLE;
		return EEXIST;
	}

	UNLOCK_ENV_VARIABLE;
	return 0;
}
