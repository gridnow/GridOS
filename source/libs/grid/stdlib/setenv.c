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

char *_environ = NULL;			//����������ָ��
char *_cur_env = NULL;			//ָ�򻷾��������пռ��׵�ַ
unsigned long _env_len = NULL;	//��ǰ��������ָ����ָ��Ŀռ���ܴ�С

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
		/* �ݲ��������·���ռ���� */
		return ENOMEM;
	}

	LOCK_ENV_VARIABLE;
	/* ��ѯ�Ƿ��Ѿ����� */
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
	if (*p == 0)			//������
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

		if (oldlen >= newlen)				//ֵ���ȱ�̣��ڴ渴�ã�����ƫ��
		{
			printf(" ���� \n ");
			memset(p, 0x00, tmplen+__SIZE_OF_END_CHAR_);
			strcpy(p, value);
		}
		else
		{
			p += tmplen + __SIZE_OF_END_CHAR_;
			memcpy(tmpp, p, _cur_env-p); 		//��ԭ���Ŀռ�ɾ��
			
			p = _cur_env - oldlen;			//���������
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
