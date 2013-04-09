#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <atomic.h>
#include <asm/ptrace.h>
#include <asm/linkage.h>

//irq_32/64.c
extern bool handle_irq(unsigned irq, struct pt_regs *regs);

//irq.c
asmregparm unsigned int do_IRQ(struct pt_regs *regs); 
extern atomic_t irq_err_count;

//i8259.c
extern void init_ISA_irqs(void);

//irqinit.c
extern void native_init_IRQ(void);
extern void arch_init_irq();


#endif /* ARCH_IRQ_H */
