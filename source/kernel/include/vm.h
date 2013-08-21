/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   内存管理
 */
#ifndef KE_VM_H
#define KE_VM_H

#include <list.h>
#include <types.h>
struct ko_process;

struct km_vm_node
{
	struct list_head node, subsection_link;
	unsigned long start, size;
};

bool km_vm_create(struct ko_process *where, struct km_vm_node *node);
bool __init km_valloc_init();

#endif
