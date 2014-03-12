/**
	The Grid Core Library
 */

/**	
	Socket for IPv4

	���ǵ�socket ͨ��Grid �����ʵ��,�����Ǽ���posix socket �Ľӿڡ�
	
	Wuxin
	82828068@qq.com
	
 */
#include <compiler.h>
#include <errno.h>
#include <socket.h>

#include "socket_file.h"

static int do_connect()
{
	return -ENOSYS;
}

static int do_send()
{
	return -ENOSYS;
}

static int do_recv()
{
	return -ENOSYS;
}

static int do_disconnect()
{
	return -ENOSYS;
}

/*
	Socket ���
*/
static struct socket_ops af_inet_socket_ops = {
	.connect 	= do_connect,
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
	�ļ����
*/
static const struct file_operations af_inet_file_ops = {
	.read		= do_file_read,
	.write		= do_file_wrte,
	.seek		= do_file_seek,
	.close		= do_file_close,
};

static int socket_inet_not_inited = 1;
void af_inet_file_init_ops(struct file *filp)
{
	struct socket_file *sf = file_get_detail(filp);
	
	filp->ops 	= &af_inet_file_ops;
	sf->ops		= &af_inet_socket_ops;

	/* But the Ipv4 module is loaded? */	
	if (unlikely(socket_inet_not_inited))
	{		
		// TODO: Load TCPIP module
	}
}

