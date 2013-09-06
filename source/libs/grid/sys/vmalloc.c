/**
	The Grid Core Library
*/

/**
	System level
	Yaosihai
*/

#include <types.h>
#include "sys/ke_req.h"

void * sys_vmalloc(xstring name, void *addr, size_t len, int prot)
{
	struct sysreq_memory_virtual_alloc req;
	
	req.base_address = (unsigned long)addr;
	req.mem_prot	 = prot;
	req.size		 = len;
	req.name		 = name;

	req.base.req_id = SYS_REQ_KERNEL_VIRTUAL_ALLOC;
	
	if (system_call(&req) == KE_INVALID_HANDLE)
	{
		return NULL;
	}

	return (void *)req.out_base;
}