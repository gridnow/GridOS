/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   X86 PAGE
 */
#ifndef ARCH_PAGE_H
#define ARCH_PAGE_H

/**
	@brief Error code of page fault
 */
#define PAGE_FAULT_P				(1<<0)
#define PAGE_FAULT_W				(1<<1)
#define PAGE_FAULT_U				(1<<2)
#define PAGE_FAULT_RESERVED			(1<<3)

/**
	@brief Define the entry flags in Hardware
 */
#define PAGE_FLAG_VALID				(1<<0)
#define PAGE_FLAG_RW				(1<<1)
#define PAGE_FLAG_USER				(1<<2)
#define PAGE_FLAG_PWT				(1<<3)
#define PAGE_FLAG_PCD				(1<<4)
#define PAGE_FLAG_ACCESSED			(1<<5)
#define PAGE_FLAG_DIRTY				(1<<6)
#define PAGE_FLAG_NOT_EXECUTABLE	(1<<63)
#define PAGE_FLAG_BIG_PAGE			(1<<7)
#define PAGE_FLAG_GLOBAL			(1<<8)
#define PAGE_FLAG_PAT				(1<<7)

#define PAGE_FLAG_FROM_OTHER		(1 << 9)
static inline void km_write_sub_table(unsigned long *table, int sub_id, unsigned long phyiscal)
{
	table[sub_id] = phyiscal | PAGE_FLAG_VALID | PAGE_FLAG_RW;
}

static inline unsigned long km_arch_get_flags(page_prot_t prot)
{
	unsigned long arch_flags = PAGE_FLAG_USER;
	if (!prot) return 0;
	
	if (prot & KM_MAP_DEVICE)
	{
		arch_flags |= PAGE_FLAG_VALID|PAGE_FLAG_RW|PAGE_FLAG_PCD|PAGE_FLAG_PWT;
	}
	
	/*Or the normal*/
	else
	{
		if (prot & KM_PROT_READ)			arch_flags |= PAGE_FLAG_VALID;
		if (prot & KM_PROT_WRITE)			arch_flags |= PAGE_FLAG_RW | PAGE_FLAG_VALID;
	}
	
	return arch_flags;
}

#endif
