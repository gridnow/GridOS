#include <linkage.h>
#include <asm/procinfo.h>

struct cpu_user_fns {
	void (*cpu_clear_user_highpage)(struct page *page, unsigned long vaddr);
	void (*cpu_copy_user_highpage)(struct page *to, struct page *from,
								   unsigned long vaddr, struct vm_area_struct *vma);
};

struct cpu_user_fns v6_user_fns  = {
	.cpu_clear_user_highpage = 0,
	.cpu_copy_user_highpage	= 0,
};
