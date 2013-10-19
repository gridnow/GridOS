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
#include "posix.h"

/**
	@brief 读取数据
 
	@param[in] void *ptr	源地址
	@param[in] void *buf	目的地址，该地址指示的空间用于存放源地址读取的数据
	@param[in] ssize_t n_bytes	需要读取的字节数
 
	@return 成功返回实际读取到的字节数，失败则为负数;
 */
typedef ssize_t (*stdio_read)	(void *ptr, void *buf, ssize_t n_bytes);

/**
	@brief 写入数据
 
	@param[in] void *ptr	目的地址
	@param[in] void *buf	源地址，该地址指示的空间用于存放需要写入目的地址的数据
	@param[in] ssize_t n_bytes	需要写入的字节数
 
	@return 成功返回实际写入的字节数，失败则为负数;
 */
typedef	ssize_t (*stdio_write)	(void *ptr, void *buf, ssize_t n_bytes);

/**
	@brief 设置偏移量
 
	@param[in] void *ptr
	@param[in] long offset	相对于whence的偏移量
	@param[in] int whence
 
	@return	成功返回新的偏移量，失败则为负数;
 */
typedef int (*stdio_seek)	(void *ptr, loff_t offset, int whence);

/**
	@brief 关闭文件
 
	@param[in] void *ptr
	
	@return	成功返回0，失败则为负数;
 */
typedef	ssize_t	(*stdio_close)	(void *ptr);

struct file_operations
{
	stdio_read		read;
	stdio_write		write;
	stdio_seek		seek;
	stdio_close		close;
};

/* Real file */
#include "stream_file.h"

#endif