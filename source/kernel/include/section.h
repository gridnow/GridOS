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

#define KS_TYPE_PRIVATE 1
#define KS_TYPE_MAP		2

struct ko_section
{
	struct km_vm_node node;
	unsigned int type;
};

void ks_init();
#endif
