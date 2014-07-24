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

/* TODO sock file lock */
#define SOCK_FILE_INIT_LOCK(sock_file)  pthread_spin_init(&sock_file->socket_lock, 0)
#define SOCK_FILE_LOCK(sock_file)       pthread_spin_lock(&sock_file->socket_lock)
#define SOCK_FILE_UNLOCK(sock_file)     pthread_spin_unlock(&sock_file->socket_lock)

bool init_socket()
{
	af_inet_init();
	return true;
}

DLLEXPORT int socket(int domain, int type, int protocol)
{	
	struct file *filp = NULL;
	int ret = -ENOSYS;
	
	switch(domain)
	{
	case AF_UNIX:
		// TODO: The local socket
		break;
		
	case AF_INET:
		{
			struct socket_file *sf;
			
			if (NULL == (filp = file_new(sizeof(struct socket_file))))
			{
				ret = -ENOMEM;
				goto err0;
			}
			sf = file_get_detail(filp);
			memset(sf, 0, sizeof(struct socket_file));
			SOCK_FILE_INIT_LOCK(sf);
			
			af_inet_file_init_ops(filp);

			if (NULL == (sf->netconn = sf->ops->socket(type, protocol)))
				goto err1;
		}
		break;		
	}

	if (filp)
	{
		ret = posix_fd_allocate(filp);
		if (ret == POSIX_INVALID_FD)
			goto err1;
	}

	return ret;
	
err1:
	filp_delete(filp);
err0:
	return ret;
}

DLLEXPORT int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	struct file *filp;
	struct socket_file *sf;
	int ret;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
		goto err0;

	sf = file_get_detail(filp);
	
	SOCK_FILE_LOCK(sf);
	ret = sf->ops->connect(sf->netconn, addr, addrlen);
	SOCK_FILE_UNLOCK(sf);
	
	return ret;
err0:
	return -EINVAL;
}

DLLEXPORT int bind(int sockfd, const struct sockaddr *addr, socklen_t addrle)
{
	struct file *filp;
	struct socket_file *sf;
	int ret;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
		return -EINVAL;

	sf = file_get_detail(filp);
	
	SOCK_FILE_LOCK(sf);
	ret = sf->ops->bind(sf->netconn, addr, addrle);
	SOCK_FILE_UNLOCK(sf);

	return ret;
}

DLLEXPORT int listen(int sockfd, int backlog)
{
	struct file *filp;
	struct socket_file *sf;
	int ret;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
		return -EINVAL;

	sf = file_get_detail(filp);
	
	SOCK_FILE_LOCK(sf);
	ret = sf->ops->listen(sf->netconn, backlog);
	SOCK_FILE_UNLOCK(sf);
	
	return ret;
}

DLLEXPORT int accept(int sockfd, const struct sockaddr *addr, socklen_t *addr_len)
{
	struct file *filp;
	struct socket_file *sfold, *sfnew;
	int ret;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
	{
		ret = -EINVAL;
		goto err;
	}

	sfold = file_get_detail(filp);
	
	SOCK_FILE_LOCK(sfold);
	/* alloc new socket des */
	if (NULL == (filp = file_new(sizeof(struct socket_file))))
	{
		ret = -ENOMEM;
		goto err;
	}
	
	sfnew = file_get_detail(filp);
	memset(sfnew, 0, sizeof(struct socket_file));
	SOCK_FILE_INIT_LOCK(sfnew);
	
	af_inet_file_init_ops(filp);

	sfnew->netconn = sfold->ops->accept(sfold->netconn, (void *)addr, (size_t *)addr_len);
	if (!(sfnew->netconn))
		goto err1;

	ret = posix_fd_allocate(filp);
	if (ret == POSIX_INVALID_FD)
		goto err1;
	
	SOCK_FILE_UNLOCK(sfold);
	return ret;
	
err1:
	filp_delete(filp);
err:
	SOCK_FILE_UNLOCK(sfold);
	return ret;
}

DLLEXPORT ssize_t send(int sockfd, const void *buf, size_t len, int flags)
{
	struct file *filp;
	struct socket_file *sf;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
		return -EINVAL;

	sf = file_get_detail(filp);

	return sf->ops->send(sf->netconn, buf, len, flags);
}

DLLEXPORT ssize_t recv(int sockfd, void *buf, size_t len, int flags)
{
	struct file *filp;
	struct socket_file *sf;
	
	/* Translate fd to standard file */
	if (NULL == (filp = posix_fd_translate(sockfd)))
		return -EINVAL;

	sf = file_get_detail(filp);

	return sf->ops->recv(sf->netconn, buf, len, flags);
}

