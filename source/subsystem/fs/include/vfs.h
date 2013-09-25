/**
	Filesystem framework
	Copyright(c) 2003-2013 Sihai(yaosihai@yeah.net). 
	All rights reserved.
*/

#ifndef FSS_VFS_H
#define FSS_VFS_H

#include <kernel/ke_atomic.h>
#include <kernel/ke_lock.h>
#include <kernel/ke_rwlock.h>

#include <list.h>
#include <ddk/vfs.h>

/************************************************************************/
/* VOLUMN                                                               */
/************************************************************************/
/**
	@brief The I/O buffer descriptor
*/
#define FSS_VOLUMN_LABEL_CHECK_LEN 5

struct fss_volumn
{
	struct list_head list;
	char volumn_id[FSS_VOLUMN_LABEL_CHECK_LEN];
	struct fss_file *root_dir;
	struct fss_vfs_driver *drv;

	/* Counter */
	struct ke_atomic using_count;
};

/**
	@brief Volumn of Unix mounting point

	Get the unix root

	@return The volumn descriptor of Unix mounting point
*/
struct fss_volumn *fss_get_unix();

/**
	@brief Create a volumn

	@return the volumn descriptor on success or NULL on failure
*/
struct fss_volumn *fss_volumn_create_simple();

/**
	@brief Delete the volumn
*/
void fss_volumn_delete(struct fss_volumn * volumn);

/**
	@brief Search the volumn for use

	Caller uses this function to find a volumn normally to analyze the path.
*/
struct fss_volumn * fss_volumn_search(xstring id);

/**
	@brief Initialize the VFS 

	Initialize the virtual file system module, this is called at the FSS starting up.
*/
void fss_init();

/**
	@brief 读取文件的某一块

	@return
		< 0 则是错误码，> 0则表示本次读取的字节数

	@note
		调用者保证buffer能容纳一个DB的大小，为了效率本函数不做检查。
*/
ssize_t fss_read(struct fss_file * who, unsigned long block, void * buffer);

/**
	@brief 打开文件
*/
struct fss_file *fss_open(char *name);

ssize_t fss_get_size(struct fss_file *who);
void fss_close(struct fss_file *who);

/************************************************************************/
/* STRING                                                               */
/************************************************************************/
#include <string.h>
#define FSS_STRLEN strlen
#define FSS_STRNCMP strncmp
#define FSS_STRCMP strcmp
#define FSS_STRCPY strcpy
#define FSS_STRNCPY strncpy
#define FSS_NAME_END_SIZE 1

/************************************************************************/
/* VFS total info                                                       */
/************************************************************************/
struct fss_vfs_info
{
	struct list_head	drv_list;

	struct ke_rwlock	vol_list_lock;
	struct list_head	vol_list;
};
#endif

/** @} */
