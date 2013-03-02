#ifndef X86_ALTERNATIVE_H
#define X86_ALTERNATIVE_H

#ifdef CONFIG_SMP
#define LOCK_PREFIX_HERE \
	".section .smp_locks,\"a\"\n"	\
	".balign 4\n"			\
	".long 671f - .\n" /* offset */	\
	".previous\n"			\
	"671:"

#define LOCK_PREFIX LOCK_PREFIX_HERE "\n\tlock; "

#else /* ! CONFIG_SMP */
#define LOCK_PREFIX_HERE ""
#define LOCK_PREFIX ""
#endif

#define OLDINSTR(oldinstr)	"661:\n\t" oldinstr "\n662:\n"
/*
 * use this macro(s) if you need more than one output parameter
 * in alternative_io
 */
#define ASM_OUTPUT2(a...) a


/* alternative assembly primitive: */
#define ALTERNATIVE(oldinstr, newinstr, feature)			\
	OLDINSTR(oldinstr)						
	
/*
 * Alternative instructions for different CPU types or capabilities.
 *
 * This allows to use optimized instructions even on generic binary
 * kernels.
 *
 * length of oldinstr must be longer or equal the length of newinstr
 * It can be padded with nops as needed.
 *
 * For non barrier like inlines please define new variants
 * without volatile and memory clobber.
 */
#define alternative(oldinstr, newinstr, feature)			\
	asm volatile (ALTERNATIVE(oldinstr, newinstr, feature) : : : "memory")

/* Like alternative_input, but with a single output argument */
#define alternative_io(oldinstr, newinstr, feature, output, input...)	\
	asm volatile (ALTERNATIVE(oldinstr, newinstr, feature)		\
		: output : "i" (0), ## input)
#endif
