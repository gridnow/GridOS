/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL-level Memory allocation
*/

#include <ddk/slab.h>

#include "memalloc.h"

//TODO: use dynamic mode
#define HAL_MEMORY_BLOCK_SIZE (32*1024)
static unsigned char block[HAL_MEMORY_BLOCK_SIZE];

void *hal_malloc(int size)
{
	return memalloc(block, size);
}

void hal_free(void * hal_address)
{
	memfree(block, hal_address);
}

bool hal_malloc_init()
{
	if (memalloc_init_allocation(block, HAL_MEMORY_BLOCK_SIZE) == false)
		return false;

	return true;	
}

