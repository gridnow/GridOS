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
void filp_delete(struct file *filp);
ke_handle filp_open(struct file *filp, const char *path, int oflags);


/************************************************************************/
/* INTERFACE                                                            */
/************************************************************************/
struct __dirstream;

/**
	@brief Read a file

	@return The bytes system has read, < 0 for error code
 */
ssize_t sys_read(struct file *filp, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

/**
	@brief 读取目录的一块

	@return
		本次读取了多少字节，并且返回下一次要从哪个文件编号开始读取。
		失败的话，返回小于0.
*/
ssize_t sys_readdir(struct __dirstream *dirp, int *next);

/**
	@brief Write a file
	
	@return The bytes of data been written, < 0 for error code
 */
ssize_t sys_write(ke_handle file, void *user_buffer, uoffset file_pos, ssize_t n_bytes);

/**
	@brief Close the kernel handle for this file
*/
int sys_close(ke_handle handle);

#endif
