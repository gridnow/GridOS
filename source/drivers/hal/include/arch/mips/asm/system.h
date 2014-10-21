#ifndef _ASM_SYSTEM_H
#define _ASM_SYSTEM_H

#include <types.h>

#include <asm/addrspace.h>
#include <asm/barrier.h>
#include <asm/cmpxchg.h>
#include <asm/cpu-features.h>
#include <asm/war.h>

static inline unsigned long __xchg_u32(volatile int * m, unsigned int val)
{
	__u32 retval;

	smp_mb__before_llsc();

	if (kernel_uses_llsc && R10000_LLSC_WAR) {
		unsigned long dummy;

		__asm__ __volatile__(
		"	.set	mips3					\n"
		"1:	ll	%0, %3			# xchg_u32	\n"
		"	.set	mips0					\n"
		"	move	%2, %z4					\n"
		"	.set	mips3					\n"
		"	sc	%2, %1					\n"
		"	beqzl	%2, 1b					\n"
		"	.set	mips0					\n"
		: "=&r" (retval), "=m" (*m), "=&r" (dummy)
		: "R" (*m), "Jr" (val)
		: "memory");
	} else if (kernel_uses_llsc) {
		unsigned long dummy;

		do {
			__asm__ __volatile__(
			"	.set	mips3				\n"
			"	ll	%0, %3		# xchg_u32	\n"
			"	.set	mips0				\n"
			"	move	%2, %z4				\n"
			"	.set	mips3				\n"
			"	sc	%2, %1				\n"
			"	.set	mips0				\n"
			: "=&r" (retval), "=m" (*m), "=&r" (dummy)
			: "R" (*m), "Jr" (val)
			: "memory");
		} while (unlikely(!dummy));
	} else {
		unsigned long flags;

		raw_local_irq_save(flags);
		retval = *m;
		*m = val;
		raw_local_irq_restore(flags);	/* implies memory barrier  */
	}

	smp_llsc_mb();

	return retval;
}

#ifdef CONFIG_64BIT
static inline u64 __xchg_u64(volatile u64 * m, u64 val)
{
	u64 retval;

	smp_mb__before_llsc();

	if (kernel_uses_llsc && R10000_LLSC_WAR) {
		unsigned long dummy;

		__asm__ __volatile__(
		"	.set	mips3					\n"
		"1:	lld	%0, %3			# xchg_u64	\n"
		"	move	%2, %z4					\n"
		"	scd	%2, %1					\n"
		"	beqzl	%2, 1b					\n"
		"	.set	mips0					\n"
		: "=&r" (retval), "=m" (*m), "=&r" (dummy)
		: "R" (*m), "Jr" (val)
		: "memory");
	} else if (kernel_uses_llsc) {
		unsigned long dummy;

		do {
			__asm__ __volatile__(
			"	.set	mips3				\n"
			"	lld	%0, %3		# xchg_u64	\n"
			"	move	%2, %z4				\n"
			"	scd	%2, %1				\n"
			"	.set	mips0				\n"
			: "=&r" (retval), "=m" (*m), "=&r" (dummy)
			: "R" (*m), "Jr" (val)
			: "memory");
		} while (unlikely(!dummy));
	} else {
		unsigned long flags;

		raw_local_irq_save(flags);
		retval = *m;
		*m = val;
		raw_local_irq_restore(flags);	/* implies memory barrier  */
	}

	smp_llsc_mb();

	return retval;
}
#else
extern u64 __xchg_u64_unsupported_on_32bit_kernels(volatile u64 * m, u64 val);
#define __xchg_u64 __xchg_u64_unsupported_on_32bit_kernels
#endif

static inline unsigned long __xchg(unsigned long x, volatile void * ptr, int size)
{
	switch (size) {
	case 4:
		return __xchg_u32(ptr, x);
	case 8:
		return __xchg_u64(ptr, x);
	}

	return x;
}

#define xchg(ptr, x)							\
({									\
	BUILD_BUG_ON(sizeof(*(ptr)) & ~0xc);				\
									\
	((__typeof__(*(ptr)))						\
		__xchg((unsigned long)(x), (ptr), sizeof(*(ptr))));	\
})


/* traps.c */
extern void set_uncached_handler(unsigned long offset, void *addr, unsigned long len);


#endif /* _ASM_SYSTEM_H */
