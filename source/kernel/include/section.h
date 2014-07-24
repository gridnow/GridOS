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
struct ko_exe;

#define KS_TYPE_PRIVATE 1
#define KS_TYPE_DEVICE	3
#define KS_TYPE_EXE		4
#define KS_TYPE_STACK	5
#define KS_TYPE_FILE	6
#define KS_TYPE_SHARE	7
#define KS_TYPE_KERNEL	8

#define KS_TYPE_MAX		10
#define KS_TYPE_MASK	0xffff
#define KS_TYPE_ADD_SUB (1<<16)

#define KM_VM_NODE_TO_SECTION(node) (struct ko_section*)(node)
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

		struct __exe
		{
			struct ko_exe *exe_object;
		} exe;

		struct __share
		{
			int size;
			int offset;
			void *src/*may be has src object*/;
			void *src_process/*the holder of the process*/;
		} share;
		
		struct __file
		{
			void *file;
		} file;
		
	} priv;
};

struct ko_section *ks_create(struct ko_process *where, unsigned long type, unsigned long base, size_t size, page_prot_t prot);
void ks_close(struct ko_process *who, struct ko_section *ks);
void ks_open_by(struct ko_process *who, struct ko_section *ks);
void ks_init();

/**
	@brief Init the section part for a process 
*/
bool ks_init_for_process(struct ko_process *who);

/**
	@brief Create a sub node on the current section

	The sub-section is the additional description for the section
*/
struct ko_section *ks_sub_create(struct ko_process * who, struct ko_section * where, unsigned long sub_address, unsigned long sub_size);

/**
	@brief Locate sub

	@note
		Unlock version!
*/
struct ko_section *ks_sub_locate(struct ko_section * where, unsigned long address);

/**
	@brief Close the subsection of a section

	@note
		Unlock version!
*/
void ks_sub_close(struct ko_process * who, struct ko_section * which);

/*
	@brief Link two section together
*/
void ks_share(struct ko_process *from, struct ko_section *where,
			  struct ko_process *to, struct ko_section *dst, int offset);

//exp.c
bool ks_exception_init();
bool ks_exception(struct ko_thread *thread, unsigned long error_address, unsigned long code);
struct ko_section *ks_get_by_vaddress(struct ko_process *where, unsigned long address);

/**
	@brief Resotore the section contents
*/
bool ks_restore_share(struct ko_process *dst_process, struct ko_section *dst_section,
					  struct ko_process *src_process, struct ko_section *src_section,
					  unsigned long to, unsigned long src, page_prot_t prot_overwrite);

// debug.c
void ks_show_by_process(struct ko_process *who);

#endif
