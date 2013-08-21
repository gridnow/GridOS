/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
*/
#ifndef FSS_H
#define FSS_H
#include <ddk/debug.h>

#define fss_debug printk
struct fss_cache_info;
struct fss_tree_info;
struct fss_vfs_info;

struct fss
{	
	struct fss_cache_info 	*cache;
	struct fss_tree_info 	*tree;
	struct fss_vfs_info 	*vfs;
};
#endif
