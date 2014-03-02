/**
	The Grid Core Library
 */

/**
	Reuqest kernel for File IO
	Zhaoyu, Yaosihai
 */
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>

#include "dir.h"
#include "file.h"
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
	ssize_t ret;
	struct sysreq_file_io req;
	
	req.base.req_id = SYS_REQ_FILE_WRITE;
	req.pos			= file_pos;
	req.file		= file;
	req.buffer		= user_buffer;
	req.size		= n_bytes;
	
	ret = system_call(&req);
	if (ret)
		return ret;
	return req.result_size;
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
	return req.result_size;
}

ssize_t sys_readdir(struct __dirstream *dirp, int *next)
{
	struct sysreq_file_readdir req;
	ssize_t ret;

	/* ��ȡһ��Ŀ¼*/	
	req.base.req_id			= SYS_REQ_FILE_READDIR;
	req.dir					= dirp->dir_handle;
	req.buffer				= dirp->dir_buffer;
	req.max_size			= dirp->total_size;
	req.start_entry			= dirp->next_bulk;
	ret = (ssize_t)system_call(&req);
	*next = req.next_entry;
	
	/* �����������ǵ������Լ����� */	
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
		/* �ļ����Ƚض�Ϊ0 */
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

