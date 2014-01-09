/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */
#include <compiler.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>

#include <ddk/debug.h>
#include "stream_file.h"

#define FLAG_LENGTH 3

/*
	���ڼ���ַ������Ƿ����ظ����ַ�
	����ֵΪtrue˵�����ظ���Ϊfalse˵�����ظ�
 
	���㷨�ÿռ任ʱ��,TODO:�д��Ż�
 */
static bool check_repeat(const char *str)
{
	/* ASCII�����ֵΪ255�����㷨�����������ռ任ȡ�㷨Ч�� */
	unsigned char ascii_array[256] = {0};
	
	while (*str != '\0')
	{
		/* �ַ�ASCII���Ӧ������Ԫ�ؼ�1 */
		ascii_array[(int)*str]++;
		
		/* ͬһ�ַ�ASCII���Ӧ������Ԫ��������ڵ���2��˵�����ظ� */
		if (ascii_array[(int)*str] >= 2) return true;
		str++;
	}
	
	return false;
}

/*
	@brief Ϊstdio_file���þ����flags
 
	r	��ֻ���ļ������ļ�������ڣ�
	r+	�򿪿ɶ�д�ļ������ļ�������ڣ�
	w	��ֻд�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
		���ļ��������򴴽��ļ���
	w+	�򿪿ɶ�д�ļ������ļ�����������ļ����ݣ����ļ�����Ϊ0��
		���ļ��������򴴽��ļ���
	a	���ӷ�ʽ��ֻд�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
		���ļ��������򴴽��ļ���
	a+	���ӷ�ʽ�򿪿ɶ�д�ļ������ļ�������д������ֻ��׷�����ļ�β���ļ�ԭ�����޷��޸ģ�
		���ļ��������򴴽��ļ���
 
	b	��������
 
	@return �ɹ�����true��ʧ���򷵻�false
 */
static bool set_file_flags(struct stdio_file *file, const char *type)
{
	bool ret = true;
	int	access_modes, status_flags;
	
	if (strlen(type) > FLAG_LENGTH) return false;
	
	/* ���flags���Ƿ����ظ����� */
	if (check_repeat(type)) return false;
	
	/* r/w/aΪ��һ���ַ� */
	switch (type[0])
	{
		case 'r':
			access_modes = O_RDONLY;
			status_flags = 0;
			break;
		case 'w':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_TRUNC;
			break;
		case 'a':
			access_modes = O_WRONLY;
			status_flags = O_CREAT | O_APPEND;
			break;
			
		default:
			return false;
	}
	
	/* 'b'��'+'λ��/r/w/a����˳��ɲ������ƣ���/r/w/ab+��/r/w/a+b��Ч */
	for (type++; *type != '\0'; type++)
	{
		switch (*type)
		{
			case 'b':
				break;
			case '+':
				access_modes = O_RDWR;
				break;
			default:
				return false;
		}
	}
	
	file->flags = access_modes | status_flags;
	return ret;
}

DLLEXPORT FILE *fopen(const char *path, const char *type)
{
	struct file *filp;
	struct stdio_file *file;
	
	filp = file_new(sizeof(struct stdio_file));
	if (!filp)
		goto err;
	file = file_get_detail(filp);
	memset(file, 0, sizeof(struct stdio_file));
	
	if (false == set_file_flags(file, type))
	{
		set_errno(EINVAL);
		goto err;
	}	
	if (KE_INVALID_HANDLE == file_open(filp, path, file->flags))
		goto err;	
	stream_file_init_ops(filp);
	
	return (FILE*)file;
	
err:
	if (filp)
		file_delete(filp);
	return NULL;
}
