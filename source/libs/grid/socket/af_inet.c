/**
	The Grid Core Library
 */

/**	
	Socket for IPv4

	我们的socket 通过Grid 网络库实现,这里是兼容posix socket 的接口。
	
	Wuxin
	82828068@qq.com
	
 */
#include <errno.h>
#include <dlfcn.h>
#include <socket.h>
#include <pthread.h>
#include <ddk/grid.h>

#include "socket_file.h"

static int socket_inet_not_inited = 1;
static pthread_spinlock_t load_lock;
static void *module_ipv4;
static struct grid_netproto *proto_ipv4;

static void *do_socket(int type, int proto)
{
	return proto_ipv4->socket(type, proto);
}

static int do_connect(struct grd_netconn *netconn, const void *addr, size_t addr_len)
{
	return proto_ipv4->connect(netconn, addr, addr_len);
}

static int do_bind(struct grd_netconn *netconn, const void *addr, size_t addr_len)
{
	return proto_ipv4->bind(netconn, (void *)addr, addr_len);
}

static int do_listen(struct grd_netconn *netconn, int backlog)
{
	return proto_ipv4->listen(netconn, backlog);
}

static void *do_accept(struct grd_netconn *netconn, void *addr, size_t *addr_len)
{
	return proto_ipv4->accept(netconn, addr, addr_len);
}

static int do_send(struct grd_netconn *netconn, void *buff, size_t len, int flag)
{
	return proto_ipv4->send(netconn, buff, len, flag);
}

static int do_recv(struct grd_netconn *netconn, void *buff, size_t len, int flag)
{
	return proto_ipv4->recv(netconn, buff, len, flag);
}

static int do_disconnect()
{
	return -ENOSYS;
}

/*
	Socket 框架
*/
static struct socket_ops af_inet_socket_ops = {
	.socket     = do_socket,
	.connect 	= do_connect,
	.bind       = do_bind,
	.listen     = do_listen,
	.accept     = do_accept,
	.send		= do_send,
	.recv		= do_recv,
	.disconnect	= do_disconnect,
};

static ssize_t do_file_read(struct file *filp, void *buf, ssize_t n_bytes)
{
	return -ENOSYS;
}

static ssize_t do_file_wrte(struct file *filp, void *buf, ssize_t n_bytes)
{
	return -ENOSYS;

}

static int do_file_seek(struct file *filp, loff_t offset, int whence)
{
	return -ENOSYS;
}

static int do_file_close(struct file *filp)
{
	return -ENOSYS;
}

/* 
	文件框架
*/
static const struct file_operations af_inet_file_ops = {
	.read		= do_file_read,
	.write		= do_file_wrte,
	.seek		= do_file_seek,
	.close		= do_file_close,
};

void af_inet_file_init_ops(struct file *filp)
{
	struct socket_file *sf = file_get_detail(filp);
	
	filp->ops 	= &af_inet_file_ops;
	sf->ops		= &af_inet_socket_ops;

	/* But the Ipv4 module is loaded? */	
	if (unlikely(socket_inet_not_inited))
	{
		pthread_spin_lock(&load_lock);
		if (!socket_inet_not_inited)
			goto next;

		if (NULL == (module_ipv4 = dlopen("tcpip.so", 0)))
			goto err0;
		if (NULL == (proto_ipv4 = dlsym(module_ipv4, GRID_GET_NETPROTO)))
			goto err1;
next:
		pthread_spin_unlock(&load_lock);
	}
	return;
	
err1:
	dlclose(module_ipv4);
	module_ipv4 = NULL;
err0:
	filp->ops = NULL;
	sf->ops = NULL;	
}

void af_inet_init()
{
	module_ipv4 = NULL;
	pthread_spin_init(&load_lock, 0);	
}

