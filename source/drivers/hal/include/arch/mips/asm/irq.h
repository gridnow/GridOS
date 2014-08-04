#ifndef ARCH_IRQ_H
#define ARCH_IRQ_H

#include <atomic.h>

/* IRQ.c */

extern atomic_t irq_err_count;
void do_IRQ(unsigned int irq); 
extern void spurious_interrupt(void);
extern void arch_init_irq();

/* MT_SMTC 对于中断的影响 */
#ifdef CONFIG_MIPS_MT_SMTC
struct irqaction;
extern unsigned long irq_hwmask[];
extern int setup_irq_smtc(unsigned int irq, struct irqaction * new,
						  unsigned long hwmask);
static inline void smtc_im_ack_irq(unsigned int irq)
{
	if (irq_hwmask[irq] & ST0_IM)
		set_c0_status(irq_hwmask[irq] & ST0_IM);
}

#else
static inline void smtc_im_ack_irq(unsigned int irq)
{
}
#endif /* CONFIG_MIPS_MT_SMTC */

/* The irq number base on CPU ，可能是个平台相关的数据 */
#ifdef CONFIG_I8259
#ifndef I8259A_IRQ_BASE
#define I8259A_IRQ_BASE	0
#endif
#endif

#ifdef CONFIG_IRQ_CPU
#ifndef MIPS_CPU_IRQ_BASE
#ifdef CONFIG_I8259
#define MIPS_CPU_IRQ_BASE 16
#else
#define MIPS_CPU_IRQ_BASE 0
#endif /* CONFIG_I8259 */
#endif
#endif /* CONFIG_IRQ_CPU */

#endif /* ARCH_IRQ_H */
