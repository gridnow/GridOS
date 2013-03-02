#ifndef X86_FIXMAP_H
#define X86_FIXMAP_H

enum fixed_addresses {
	FIX_APIC_BASE,
	__end_of_fixed_addresses
};

#define FIXADDR_TOP 0xfffff000

#define __fix_to_virt(x)	(FIXADDR_TOP - ((x) << PAGE_SHIFT))
#define __virt_to_fix(x)	((FIXADDR_TOP - ((x)&PAGE_MASK)) >> PAGE_SHIFT)

static __always_inline unsigned long fix_to_virt(const unsigned int idx)
{
	if (idx >= __end_of_fixed_addresses)
		return 0;

	return __fix_to_virt(idx);
}

#endif
