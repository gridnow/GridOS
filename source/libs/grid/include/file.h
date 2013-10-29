/**
	The Grid Core Library
 */

/**
	Posix file
	ZhaoYu,Yaosihai
 */

#ifndef __CRT_FILE_H__
#define __CRT_FILE_H__

#include <types.h>
#include <kernel/ke_srv.h>

struct file;

struct file_operations
{
	/**
		读取数据
		成功返回实际读取到的字节数，失败则为负错误码;
	*/
	ssize_t (*read)	(struct file *filp, void *buf, ssize_t n_bytes);
	
	/**
		写入数据
		成功返回实际写入的字节数，失败则为负错误码;
	*/
	ssize_t (*write)(struct file *filp, void *buf, ssize_t n_bytes);
	
	/**
		设置偏移量
		成功返回0，失败则为负错误码;
	*/
	int (*seek)(struct file *filp, loff_t offset, int whence);

	/**
		关闭文件
		成功返回0，失败则为负错误码;
	*/
	int	(*close)(struct file *filp);
};

struct file
{
	const struct file_operations *ops;
	uoffset						pos;
	lsize_t						size;
	ke_handle					handle;
	void *detailed;
};

static inline void *file_get_detail(struct file *filp)
{
	return filp + 1;
}

static inline struct file *file_get_from_detail(void *detail_file)
{
	return (void*)(unsigned long)detail_file - sizeof(struct file);
}

struct file *file_new(int detail_size);
void file_delete(struct file *filp);
ke_handle file_open(struct file *filp, const char *path, int oflags);

#endif