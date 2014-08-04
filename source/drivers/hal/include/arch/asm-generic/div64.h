#ifndef _ASM_GENERIC_DIV64_H
#define _ASM_GENERIC_DIV64_H
/*
 * Copyright (C) 2003 Bernardo Innocenti <bernie@develer.com>
 * Based on former asm-ppc/div64.h and asm-m68knommu/div64.h
 *
 * The semantics of do_div() are:
 *
 * u32 do_div(u64 *n, u32 base)
 * {
 * 	u32 remainder = *n % base;
 * 	*n = *n / base;
 * 	return remainder;
 * }
 *
 * NOTE: macro parameter n is evaluated multiple times,
 *       beware of side effects!
 */

#include <types.h>
#include <compiler.h>

#if BITS_PER_LONG == 64

# define do_div(n,base) ({					\
	u32 __base = (base);				\
	u32 __rem;						\
	__rem = ((u64)(n)) % __base;			\
	(n) = ((u64)(n)) / __base;				\
	__rem;							\
 })

#elif BITS_PER_LONG == 32

extern u32 __div64_32(u64 *dividend, u32 divisor);

/* The unnecessary pointer compare is there
 * to check for type safety (n must be 64bit)
 */
# define do_div(n,base) ({				\
	u32 __base = (base);			\
	u32 __rem;					\
	(void)(((typeof((n)) *)0) == ((u64 *)0));	\
	if (likely(((n) >> 32) == 0)) {			\
		__rem = (u32)(n) % __base;		\
		(n) = (u32)(n) / __base;		\
	} else 						\
		__rem = __div64_32(&(n), __base);	\
	__rem;						\
 })

#else /* BITS_PER_LONG == ?? */

# error do_div() does not yet support the C64

#endif /* BITS_PER_LONG */

#endif /* _ASM_GENERIC_DIV64_H */
