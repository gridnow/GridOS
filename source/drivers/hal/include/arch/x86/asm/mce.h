#ifndef _ASM_X86_MCE_H
#define _ASM_X86_MCE_H

#ifdef CONFIG_X86_MCE
int mcheck_init(void);
void mcheck_cpu_init(struct cpuinfo_x86 *c);
#else
static inline int mcheck_init(void) { return 0; }
static inline void mcheck_cpu_init(struct cpuinfo_x86 *c) {}
#endif

#endif
