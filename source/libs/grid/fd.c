/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
	The Grid Core Library

	Wuxin (82828068@qq.com)
*/
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

#include "posix.h"

#include "i2p.h"

static void *fd_table;
static pthread_spinlock_t fd_table_lock;

static void * get_handle_table()
{	
	pthread_spin_lock(&fd_table_lock);
	return fd_table;
}

static void put_handle_table()
{
	pthread_spin_unlock(&fd_table_lock);
}

static void * handle_space_alloc(size_t size)
{
	void * p = malloc(size);
	return p;
}

static void handle_space_free(void * p)
{
	free(p);
}

int posix_fd_allocate(void *object)
{
	i2p_handle h;
	void * fd_table;
	
	fd_table = get_handle_table();
	
	/* Sanity */
	if (fd_table == NULL)
		return POSIX_INVALID_FD;
	
	h = i2p_alloc(fd_table, object);
	if (h == COMMON_I2P_ALLOC_ERROR)
	{
		h = POSIX_INVALID_FD;
		goto end;
	}
	
end:
	put_handle_table();
	return h;
}

bool posix_deallocate_fd(int fd)
{
	void *fd_table;
	bool r;
	
	fd_table = get_handle_table();
	r = i2p_dealloc(fd_table, fd);
	put_handle_table();
	
	return r;
}

bool posix_fd_init()
{
	fd_table = i2p_create(handle_space_alloc, handle_space_free);
	if (fd_table)
		return false;
	pthread_spin_init(&fd_table_lock, 0);
	return true;
}

void posix_fd_deinit()
{	
	if (fd_table)
	{
		i2p_delete(fd_table);
		fd_table = NULL;
	}
}

void *posix_fd_translate(int fd)
{
	void *object;
	void *fd_table;
	
	fd_table = get_handle_table();
	object = i2p_find(fd_table, fd);	
	put_handle_table();
	
	return object;
}

