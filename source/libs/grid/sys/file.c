/**
	The Grid Core Library
 */

/**
	Reuqest kernel for File IO
	Zhaoyu, Yaosihai
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

#include <ystd.h>

#include "dir.h"
#include "file.h"
#include "stream_file.h"
#include "nocache_file.h"
#include "sys/file_req.h"

static ke_handle sys_mkfile(const char *name)
{
	struct sysreq_file_create req;
	
	req.base.req_id = SYS_REQ_FILE_CREATE;
	req.name		= (char *)name;
	
	return system_call(&req);
}

static int sys_ftruncate(ke_handle file, ssize_t length)
{
	struct sysreq_file_ftruncate req;
	int ret ;
	
	req.base.req_id		= SYS_REQ_FILE_FTRUNCATE;
	req.file			= file;
	req.length			= length;
	
	ret = system_call(&req);
	
	return ret;
}

static ke_handle sys_open(const char *name, lsize_t *size)
{
	ke_handle h;
	struct sysreq_file_open req;
	
	req.base.req_id = SYS_REQ_FILE_OPEN;
	req.name		= (char *)name;
	
	h = system_call(&req);
	if (size)
		*size = req.file_size;

	return h;
}

int sys_close(ke_handle handle)
{
	struct sysreq_file_close req;
	
	req.base.req_id = SYS_REQ_FILE_CLOSE;
	req.file 		= handle;
	
	system_call(&req);
	return 0;
}

ssize_t sys_write(ke_handle file, void *user_buffer, uoffset file_pos, ssize_t n_bytes)
{
	struct sysreq_file_io req;
	
	req.base.req_id = SYS_REQ_FILE_WRITE;
	req.pos			= file_pos;
	req.file		= file;
	req.buffer		= user_buffer;
	req.size		= n_bytes;
	
	return (ssize_t)system_call(&req);
}

ssize_t sys_read(struct file *filp, void *user_buffer, uoffset file_pos, ssize_t n_bytes)
{
	ssize_t ret;
	struct sysreq_file_io req;
	
	req.base.req_id = SYS_REQ_FILE_READ;
	req.pos			= file_pos;
	req.file		= filp->handle;
	req.buffer		= user_buffer;
	req.size		= n_bytes;
	
	ret = system_call(&req);
	if (ret < 0)
		return ret;

	/* Read file will update user space file size */
	filp->size = req.current_size;
	return ret;
}

ssize_t sys_readdir(struct __dirstream *dirp, int *next)
{
	struct sysreq_file_readdir req;
	ssize_t ret;

	/* 读取一块目录*/	
	req.base.req_id			= SYS_REQ_FILE_READDIR;
	req.dir					= dirp->dir_handle;
	req.buffer				= dirp->dir_buffer;
	req.max_size			= dirp->total_size;
	req.start_entry			= dirp->next_bulk;
	ret = (ssize_t)system_call(&req);
	*next = req.next_entry;
	
	/* 解析的事情是调用者自己负责 */	
	return ret;
}

/************************************************************************/
/* Common for all types of file                                         */
/************************************************************************/
struct file *file_new(int detail_size)
{
	struct file *filp;
	int size = detail_size + sizeof(*filp);
	
	filp = malloc(size);
	if (!filp)
		goto end_exit;
	memset(filp, 0, sizeof(*filp));
	
	return filp;
	
end_exit:
	return NULL;
}

void filp_delete(struct file *filp)
{
	free(filp);
}

ke_handle filp_open(struct file *filp, const char *path, int oflags)
{
	ke_handle file_handle;
	
	file_handle = sys_open(path, &filp->size);
	if (KE_INVALID_HANDLE == file_handle)
	{
		if (oflags & O_CREAT)
			file_handle = sys_mkfile(path);
		
		if (KE_INVALID_HANDLE == file_handle)
			goto err;
	}
	else if (oflags & O_TRUNC)
	{
		/* 文件长度截短为0 */
		if (sys_ftruncate(file_handle, 0))
			goto err;
		filp->size = 0;
	}
	
	filp->handle = file_handle;
	return file_handle;
	
err:
	if (KE_INVALID_HANDLE != file_handle)
		sys_close(file_handle);

	return KE_INVALID_HANDLE;
}

ke_handle dir_open(const char *path)
{
	ke_handle file_handle;
	
	file_handle = sys_open(path, NULL);
	if (KE_INVALID_HANDLE == file_handle)
		goto err;
	
	return file_handle;
	
err:
	if (KE_INVALID_HANDLE != file_handle)
		sys_close(file_handle);

	return KE_INVALID_HANDLE;
}

/************************************************************************/
/* Native file interface                                         */
/************************************************************************/
DLLEXPORT y_handle y_file_open(const char *path, y_file_operation_type_t ops_type)
{
	y_handle h;
	struct stdio_file *filp;
	struct file *file = NULL;
	
	//TODO: 增加更复杂的类型
	//TODO: 根据path 来确定文件类型
	filp = (struct stdio_file *)fopen(path, "r+");
	if (!filp)
		goto err;


	file = file_get_from_detail(filp);
	
	/* init file operations */
	switch(ops_type)
	{
		case Y_FILE_OPERATION_CACHE:
			stream_file_init_ops(file);
			break;

		case Y_FILE_OPERATION_NOCACHE:
			nocache_file_init_ops(file);
			break;

		default:
			goto err;
	}
	
	h = (y_handle)filp;
	return h;
err:
	if(filp)
		y_file_close((y_handle)filp);
	h = Y_INVALID_HANDLE;
	return h;
}

DLLEXPORT ssize_t y_file_read(y_handle file, void *buffer, size_t size)
{	
	ssize_t ret;
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);

	ret = filp->ops->read(filp, buffer, size);
	return ret;
}

DLLEXPORT ssize_t y_file_write(y_handle file, void *buffer, size_t size)
{	
	ssize_t ret;
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);

	ret = filp->ops->write(filp, buffer, size);
	return ret;
}

DLLEXPORT void y_file_close(y_handle file)
{	
	fclose((void*)file);	
}

DLLEXPORT int y_file_seek(y_handle file, loff_t where, int whence)
{	
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);

	return filp->ops->seek(filp, where, whence);
}

DLLEXPORT int y_file_event_register(y_handle file, y_file_event_type_t event_mask, void *func, void *para)
{
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);
	struct sysreq_file_notify req;
	
	req.base.req_id = SYS_REQ_FILE_NOTIFY;
	req.ops 		= SYSREQ_FILE_OPS_REG_FILE_NOTIFY;
	req.file		= filp->handle;
	req.ops_private.reg.mask = event_mask;
	req.ops_private.reg.func = func;
	req.ops_private.reg.para = para;
	
	return system_call(&req);	
}

DLLEXPORT int y_file_event_unregister(y_handle file, y_file_event_type_t event_mask)
{
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);
	struct sysreq_file_notify req;
	
	req.base.req_id = SYS_REQ_FILE_NOTIFY;
	req.ops 		= SYSREQ_FILE_OPS_UNREG_FILE_NOTIFY;
	req.file		= filp->handle;
	req.ops_private.unreg.mask = event_mask;
	
	return system_call(&req);
}

DLLEXPORT void * y_file_mmap(y_handle file, size_t len, page_prot_t prot, int flags, off_t offset)
{
	struct stdio_file *f = (struct stdio_file*)file;
	struct file *filp = file_get_from_detail(f);
	struct sysreq_file_map req;

	req.base.req_id = SYS_REQ_FILE_MAP;
	req.ops         = SYSREQ_FILE_OPS_MAP;
	req.file        = filp->handle;
	req.prot        = prot;
	
	if (system_call(&req) < 0)
		return NULL;

	return req.map_base;
}

DLLEXPORT int y_file_munmap(y_handle file, void *addr, size_t length)
{
	//TODO
	return ENOSYS;
}


