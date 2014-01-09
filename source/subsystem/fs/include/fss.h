/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
 
	Interface layer
*/
#ifndef FSS_H
#define FSS_H

#include <ddk/debug.h>
#define fss_debug printk

struct fss_file;
struct fss_cache_info;
struct fss_tree_info;
struct fss_vfs_info;

struct fss
{	
	struct fss_cache_info 	*cache;
	struct fss_tree_info 	*tree;
	struct fss_vfs_info 	*vfs;
};

/* MAP.C */
/**
	@brief 准备好DBD/DMR等关键数据结构
 
	@return
		DB 地址
	@note
		本函数保证DB中有有效数据
*/
void *fss_map_prepare_dbd(struct fss_file *file, void *process, uoffset file_pos);

/* MAIN.c */
/**
	@brief 读取文件的某一块
 
	@return
		< 0 则是错误码，> 0则表示本次读取的字节数
 
	@note
		调用者保证buffer能容纳一个DB的大小，为了效率本函数不做检查。
 */
ssize_t fss_read(struct fss_file * who, unsigned long block, void * buffer);

/**
 */
struct fss_file *fss_open(struct fss_file *current_dir, char *name);

/**
 */
lsize_t fss_get_size(struct fss_file *who);

/**
 */
void fss_close(struct fss_file *who);

#endif
