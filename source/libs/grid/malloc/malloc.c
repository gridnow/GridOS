/**
	The Grid Core Library
*/

/**
	Posix memory allocation
	Yaosihai
*/

#include <compiler.h>
#include <types.h>
#include <stdio.h>

#include <sys/mman.h>
#include "malloc.h"
#include "memalloc.h"
#include "cl_string.h"

#define GLOBAL_BLOCK_SIZE 1024*1024*8
#define LOCK_MALLOC()
#define UNLOCK_MALLOC()
static void *global_block;

bool init_malloc()
{
	bool ret;

	global_block = mmap(0, GLOBAL_BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
	if (MAP_FAILED == global_block) return false;

	ret = memalloc_init_allocation(global_block, GLOBAL_BLOCK_SIZE);

	return ret;
}

DLLEXPORT void * malloc(size_t _Size)
{
	void *p = NULL;

	if (global_block)
	{
		LOCK_MALLOC();
		p = memalloc(global_block, _Size);
		UNLOCK_MALLOC();
	}
	
	return p;
}

DLLEXPORT void free(void * p)
{
	void *ret;
	
	LOCK_MALLOC();
	ret = memfree(global_block, p);
	UNLOCK_MALLOC();
	
	if (!ret)
	{
		printf("memfree error\n\r");
	}
}

/************************************************************************/
/* Internal use                                                         */
/************************************************************************/
void *crt_free(void *p)
{
	free(p);
}

void *crt_alloc(int size)
{
	return malloc(size);
}

void *crt_zalloc(int size)
{
	void *p = crt_alloc(size);
	if (p)
		memset(p, 0, size);
	return p;
}
