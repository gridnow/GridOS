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
		维护一个缓冲区，一块一块的读取目录内容。
	*/
	void 				*dir_buffer;							/* 我们从底层读取一批文件上来 */
	void				*current_entry;							/* 某些接口要遍历本块的entry */
	int 				total_size, used_size;			
	int					next_bulk;								/* 本块的起始文件编号 */
	
	/*
		目录句柄。
	*/
	ke_handle			dir_handle;

	/* 
		记录一个零时的，为readdir等接口使用。
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

