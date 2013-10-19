/**
	The Grid Core Library
 */

/**
	Reuqest kernel for File IO
	Zhaoyu, Yaosihai
 */
#include <errno.h>

#include "file.h"
#include "sys/file_req.h"

int sys_open(const char *name, const unsigned int file_type)
{
	struct sysreq_file_open req;
	
	req.base.req_id = SYS_REQ_FILE_OPEN;
	req.file_type	= file_type;
	req.name		= (char *)name;
	req.fd			= POSIX_INVALID_FD;
	
	system_call(&req);
	
	return req.fd;
}

int sys_close(int fd)
{
	return ENOSYS;
}

int sys_mkfile(const char *name)
{
	struct sysreq_file_create req;
	
	req.base.req_id = SYS_REQ_FILE_CREATE;
	req.name		= (char *)name;
	req.fd			= POSIX_INVALID_FD;
	
	system_call(&req);
	
	return req.fd;
}

int sys_ftruncate(int fd, ssize_t length)
{
	struct sysreq_file_ftruncate req;
	int ret ;
	
	req.base.req_id		= SYS_REQ_FILE_FTRUNCATE;
	req.fd				= fd;
	req.length			= length;
	
	ret = system_call(&req);
	
	return ret;
}

size_t sys_write(int fd, void *user_buffer, uoffset file_pos, ssize_t n_bytes)
{
	struct sysreq_file_io req;
	
	req.base.req_id = SYS_REQ_FILE_WRITE;
	req.pos			= file_pos;
	req.fd			= fd;
	req.buffer		= user_buffer;
	req.size		= n_bytes;
	
	system_call(&req);
	
	return req.result_size;
}

size_t sys_read(int fd, void *user_buffer, uoffset file_pos, ssize_t n_bytes)
{
	struct sysreq_file_io req;
	
	req.base.req_id = SYS_REQ_FILE_READ;
	req.pos			= file_pos;
	req.fd			= fd;
	req.buffer		= user_buffer;
	req.size		= n_bytes;
	
	system_call(&req);
	
	return req.result_size;
}