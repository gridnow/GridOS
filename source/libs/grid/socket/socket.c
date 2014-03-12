/**
	The Grid Core Library
 */

/**
	Socket subystem
	Yaosihai
 */
#include <compiler.h>
#include <errno.h>
#include <socket.h>
#include <string.h>

#include "socket_file.h"

DLLEXPORT int socket(int domain, int type, int protocol)
{
	int ret = -ENOSYS;
	
	switch(domain)
	{
	case AF_UNIX:
		// TODO: The local socket
		break;
		
	case AF_INET:
		{
			struct file *filp;
			struct socket_file *sf;
			
			if (NULL == (filp = file_new(sizeof(struct socket_file))))
				goto err0;
			sf = file_get_detail(filp);
			memset(sf, 0, sizeof(struct socket_file));
			af_inet_file_init_ops(filp);
		}
		break;		
	}

	return ret;
	
err0:
	return ret;
}

DLLEXPORT int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	struct file *filp;
	struct socket_file *sf;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_translate_fd(sockfd)))
		goto err0;

	sf = file_get_detail(filp);
	return sf->ops->connect(addr, addrlen);
	
err0:
	return -EINVAL;
}

DLLEXPORT ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
	return -ENOSYS;
}

DLLEXPORT ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
	return -ENOSYS;
}

