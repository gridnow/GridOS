/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内核地址空间的虚拟内存分配
 */

#include <types.h>
#include <kernel/ke_memory.h>

#include <spinlock.h>

#include "memalloc.h"

/*
	@brief Virtual memory used only by kernel
 */
#define KE_VIRTUAL_MEMORY_SIZE (13*1024*1024)

static void *kernel_vm_object;
static spinlock_t kernel_vm_object_lock;

bool __init km_valloc_init()
{
	kernel_vm_object = km_alloc_virtual(KE_VIRTUAL_MEMORY_SIZE, KM_PROT_READ | KM_PROT_WRITE, NULL);
	if (!kernel_vm_object)
		goto err;
	
	spin_lock_init(&kernel_vm_object_lock);
	memalloc_init_allocation(kernel_vm_object, KE_VIRTUAL_MEMORY_SIZE);
	return true;
	
err:
	return false;
}

/************************************************************************/
/* INTERFACE                                                            */
/************************************************************************/
void *km_valloc(unsigned long size)
{
	void *p;
	spin_lock(&kernel_vm_object_lock);
	p = memalloc(kernel_vm_object, size);
	spin_unlock(&kernel_vm_object_lock);
	return p;
}

void *km_vfree(void *kp)
{
	spin_lock(&kernel_vm_object_lock);
	memfree(kernel_vm_object, kp);
	spin_unlock(&kernel_vm_object_lock);

	return kp;
}