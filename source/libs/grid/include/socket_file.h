/**
	The Grid Core Library
 */

/**
	Socket file

	一个Socket 文件为用户提供一个访问socket数据的节点
	Wuxin
 */

#ifndef __SOCKET_FILE_H__
#define __SOCKET_FILE_H__

#include "file.h"
#include "posix.h"

struct socket_ops
{
	int (*connect)(const void *addr, size_t addr_len);
	int (*send)();
	int (*recv)();
	int (*disconnect)();
};

/*
	文件层
*/
struct socket_file
{
	struct socket_ops *ops;
};

bool init_socket();

/* Socket type ops */
void af_inet_file_init_ops(struct file *filp);
void af_inet_init();

#endif
