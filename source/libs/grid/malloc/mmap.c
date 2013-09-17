/**
	The Grid Core Library
*/

/**
	Posix memory map
	Yaosihai
*/
#include <compiler.h>
#include <sys/mman.h>

static void *mmap64(void *addr, size_t len, int prot, int flags, int fd, loff_t offset)
{
	void * ret = NULL;

	/* 常规开辟虚拟内存 */
	if (-1 == fd)
	{
		ret = sys_vmalloc(NULL, addr, len, prot);
	}

	if (!ret) ret = MAP_FAILED;

	return ret;
}

DLLEXPORT void *mmap(void *addr, size_t len, int prot, int flags, int fd, off_t offset)
{
	return mmap64(addr, len, prot, flags, fd, (loff_t)offset);
}
