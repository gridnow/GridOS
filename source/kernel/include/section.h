/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ÄÚ´æ¹ÜÀí
 */
#ifndef KE_SECTION_H
#define KE_SECTION_H
#include <kernel/kernel.h>

#include "vm.h"
struct ko_process;
struct ko_thread;

#define KS_TYPE_PRIVATE 1
#define KS_TYPE_MAP		2
#define KS_TYPE_DEVICE	3
#define KS_TYPE_EXE		4
#define KS_TYPE_STACK	5
#define KS_TYPE_FILE	6
#define KS_TYPE_SHARE	7
#define KS_TYPE_KERNEL	8
#define KS_TYPE_MAX		9
#define KS_TYPE_MASK	0xffff

struct ko_section
{
	struct km_vm_node node;
	unsigned int type;
	page_prot_t prot;
	
	union __detailed__
	{
		struct __physical_mapping__
		{
			u64 base;
		} phy;
		
	} priv;
};

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, unsigned long size, page_prot_t prot);
void ks_close(struct ko_section *ks);
void ks_init();

//exp.c
bool ks_exception_init();
bool ks_exception(struct ko_thread *thread, unsigned long error_address, unsigned long code);

#endif
