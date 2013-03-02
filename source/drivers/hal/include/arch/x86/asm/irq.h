#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H
#include <asm/ptrace.h>
#include <asm/linkage.h>

extern bool handle_irq(unsigned irq, struct pt_regs *regs);
asmregparm unsigned int do_IRQ(struct pt_regs *regs); 
extern void arch_init_irq();

#endif /* ARCH_IRQ_H */
