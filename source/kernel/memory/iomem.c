/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   –Èƒ‚µÿ÷∑π‹¿Ì∆˜
 */

#include <types.h>
#include <kernel/ke_memory.h>

#include "process.h"
#include "section.h"

static bool sync_mapping(unsigned long v, phy_t p, size_t l)
{
	/* TODO: Walk to map physical to virtual */
	
	return true;
}

void *km_map_physical(phy_t physical, size_t size, unsigned int flags)
{
	struct ko_section *ks;
	
	ks = ks_create(kp_get_system(), KS_TYPE_DEVICE, 0, size);
	if (!ks)
		goto err;
	printk("km_map_physical got virtual start = %p, size = %d, mapping physical...", ks->node.start, ks->node.size);
	
	if (sync_mapping(ks->node.start, physical, ks->node.size) == false)
		goto err1;
	
	return (void*)ks->node.start;
	
err1:
	ks_close(ks);
err:
	return NULL;
}