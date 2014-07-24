/**
	The Grid Core Library
 */

/**
	Socket file

	һ��Socket �ļ�Ϊ�û��ṩһ������socket���ݵĽڵ�
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
	�ļ���
*/
struct socket_file
{
	struct socket_ops *ops;
	struct grd_netconn *netconn;
	pthread_spinlock_t socket_lock;
};

bool init_socket();

/* Socket type ops */
void af_inet_file_init_ops(struct file *filp);
void af_inet_init();

#endif
