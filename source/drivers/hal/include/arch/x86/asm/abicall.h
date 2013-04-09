#ifndef ARCH_ASM_ABICALL_H
#define ARCH_ASM_ABICALL_H

#define ka_call_dynamic_module_entry(entry, ...) \
	({	unsigned long __res;								\
	__res = (unsigned long)entry(__VA_ARGS__); \
})


#endif