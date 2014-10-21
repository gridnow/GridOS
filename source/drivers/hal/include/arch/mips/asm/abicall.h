#ifndef ARCH_ASM_ABICALL_H
#define ARCH_ASM_ABICALL_H
/************************************************************************/
/* MISC                                                                 */
/************************************************************************/
#define ka_call_dynamic_module_entry(entry, ...) \
({	unsigned long __res;								\
	/* Set prologue */	\
	__asm__ __volatile__(						\
	".set\tnoreorder\n\t"	\
	"move\t$t9,%0 \t\n"	\
	".set\treorder\n\t"	\
	:: "Jr"(entry): "t9", "memory"	\
	);	\
	\
	__res = (unsigned long)entry(__VA_ARGS__); \
})

#endif