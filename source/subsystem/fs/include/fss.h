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
	@brief ׼����DBD/DMR�ȹؼ����ݽṹ
 
	@return
		DB ��ַ
	@note
		��������֤DB������Ч����
*/
void *fss_map_prepare_dbd(struct fss_file *file, void *process, uoffset file_pos);

/* MAIN.c */
/**
	@brief ��ȡ��д���ļ���ĳһ��
 
	@return
		< 0 ���Ǵ����룬> 0���ʾ���ζ�ȡ���ֽ���
 
	@note
		�����߱�֤buffer������һ��DB�Ĵ�С��Ϊ��Ч�ʱ�����������顣
 */
ssize_t fss_block_io(struct fss_file * who, unsigned long block, void * buffer, bool write);

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
