/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
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
struct fss_file *fss_open(struct fss_file *current_dir, char *name);
ssize_t fss_get_size(struct fss_file *who);
void fss_close(struct fss_file *who);


#endif
