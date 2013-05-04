/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内存管理
 */
#ifndef KE_SECTION_H
#define KE_SECTION_H

#include "vm.h"
struct ko_process;

#define KS_TYPE_PRIVATE 1
#define KS_TYPE_MAP		2
#define KS_TYPE_DEVICE	3
#define KS_TYPE_MASK	0xffff


struct ko_section
{
	struct km_vm_node node;
	unsigned int type;
	
	union __detailed__
	{
		struct __physical_mapping__
		{
			u64 base;
		} phy;
		
	} priv;
};

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, unsigned long size,...);
void ks_close(struct ko_section *ks);
void ks_init();


#endif
