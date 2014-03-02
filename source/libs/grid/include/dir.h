/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#ifndef __CRT_DIR_H__
#define __CRT_DIR_H__

#include <kernel/ke_srv.h>

struct __dirstream
{
	/*
		ά��һ����������һ��һ��Ķ�ȡĿ¼���ݡ�
	*/
	void 				*dir_buffer;							/* ���Ǵӵײ��ȡһ���ļ����� */
	void				*current_entry;							/* ĳЩ�ӿ�Ҫ���������entry */
	int 				total_size, used_size;			
	int					next_bulk;								/* �������ʼ�ļ���� */
	
	/*
		Ŀ¼�����
	*/
	ke_handle			dir_handle;

	/* 
		��¼һ����ʱ�ģ�Ϊreaddir�Ƚӿ�ʹ�á�
	*/	
	union 														
	{
		struct dirent64	dentry64;
		struct dirent	dentry;
	} entry_data;
};

/* About dir */
ke_handle dir_open(const char *path);

#endif

