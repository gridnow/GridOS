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

#include <list.h>
#include <ddk/grid.h>

#include "file.h"
#include "posix.h"

struct socket_ops
{
	void *(*socket)(int proto, int type);
	int (*connect)(struct grd_netconn *netconn, const void *addr, size_t addr_len);
	int (*bind)(struct grd_netconn *netconn, const void *addr, size_t addr_len);
	int (*listen)(struct grd_netconn *netconn, int backlog);
	void *(*accept)(struct grd_netconn *netconn, void *addr, size_t *addr_len);
	int (*send)(struct grd_netconn *netconn, void *buff, size_t len, int flag);
	int (*recv)(struct grd_netconn *netconn, void *buff, size_t len, int flag);
	int (*disconnect)();
};

/*
	文件层
*/
struct socket_file
{
	struct socket_ops *ops;
	struct grd_netconn *netconn;
};

bool init_socket();

/* Socket type ops */
void af_inet_file_init_ops(struct file *filp);
void af_inet_init();

#endif
