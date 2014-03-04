/**
	The Grid Core Library
*/

/**
	Posix directory API
	Yaosihai,ZhaoYu
*/

#include <dirent.h>
#include <string.h>

#include "posix.h"
#include "file.h"
#include "dir.h"

#include "sys/file_req.h"

static void fill_32bits_dirent(struct dirent_buffer *entry, struct dirent *dentry)
{
	int name_length = entry->name_length >= NAME_MAX ? (NAME_MAX - POSIX_END_CHAR_SIZE) : entry->name_length;
	dentry->d_name[name_length] = '\0'; 
	strncpy(dentry->d_name, entry->name, name_length);
}

static void fill_64bits_dirent(struct dirent_buffer *entry, struct dirent64 *dentry)
{
	int name_length = entry->name_length >= NAME_MAX ? (NAME_MAX - POSIX_END_CHAR_SIZE) : entry->name_length;
	dentry->d_name[name_length] = '\0';  
	strncpy(dentry->d_name, entry->name, name_length);
}

static void *read_dir (DIR *dirp, int bits)
{
	int next;
	ssize_t bulk;
	void *actual_dentry;
	struct dirent_buffer *entry;

	/*
		找到当前的项目，没有则读取目录内容—块。
	*/
	if (dirp->current_entry == NULL)
	{
		if ((bulk = sys_readdir(dirp, &next)) <= 0)
			goto err;
		dirp->current_entry 	= dirp->dir_buffer;
		dirp->used_size			= bulk;
		dirp->next_bulk			= next;
	}

	entry = dirp->current_entry;
	if (32 == bits)
	{
		actual_dentry = &dirp->entry_data.dentry;
		fill_32bits_dirent(entry, actual_dentry);
			
	}
	else if (64 == bits)
	{
		actual_dentry = &dirp->entry_data.dentry64;
		fill_64bits_dirent(entry, actual_dentry);
	}
	else
		goto err;

	/* Locate to next */
	entry = (void*)entry + sizeof(*entry) + entry->name_length /* Next entry with dynamic size */;
	if ((unsigned long)entry >= 
		(unsigned long)dirp->dir_buffer /*start*/ + dirp->used_size/*current block size*/)
	{		
		/* Set to no entry, we will reread a new block again */
		dirp->current_entry = NULL;		
	}
	else
		dirp->current_entry = entry;
	
	return actual_dentry;
err:
	return NULL;		
}

/**
	@brief 目录项读取操作

	@return 成功返回目录项dirent指针，失败或达到目录结尾返回NULL
*/
DLLEXPORT struct dirent *readdir(DIR *dirp)
{
	return read_dir(dirp, 32);
}

/**
	@brief 目录项读取操作

	@return 成功返回目录项dirent指针，失败或达到目录结尾返回NULL
*/
DLLEXPORT struct dirent64 *readdir64(DIR *dirp)
{
	return read_dir(dirp, 64);
}


