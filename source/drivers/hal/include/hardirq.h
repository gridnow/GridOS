/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Ӳ���жϼ���������
*/
#ifndef HAL_HARDIRQ_H
#define HAL_HARDIRQ_H

#include "preempt.h"

/* preempt �������ֶ�����
* PREEMPT_MASK: 0x000000ff
* SOFTIRQ_MASK: 0x0000ff00
* HARDIRQ_MASK: 0x03ff0000
*	  NMI_MASK: 0x04000000
*/
#define PREEMPT_BITS	8
#define SOFTIRQ_BITS	8
#define NMI_BITS	1
#define MAX_HARDIRQ_BITS 10
#ifndef HARDIRQ_BITS
# define HARDIRQ_BITS	MAX_HARDIRQ_BITS
#endif
#if HARDIRQ_BITS > MAX_HARDIRQ_BITS
#error HARDIRQ_BITS too high!
#endif

#define IRQ_EXIT_OFFSET (HARDIRQ_OFFSET-1)

#define PREEMPT_SHIFT	0
#define SOFTIRQ_SHIFT	(PREEMPT_SHIFT + PREEMPT_BITS)
#define HARDIRQ_SHIFT	(SOFTIRQ_SHIFT + SOFTIRQ_BITS)
#define NMI_SHIFT	(HARDIRQ_SHIFT + HARDIRQ_BITS)

#define __IRQ_MASK(x)	((1UL << (x))-1)

#define PREEMPT_MASK	(__IRQ_MASK(PREEMPT_BITS) << PREEMPT_SHIFT)
#define SOFTIRQ_MASK	(__IRQ_MASK(SOFTIRQ_BITS) << SOFTIRQ_SHIFT)
#define HARDIRQ_MASK	(__IRQ_MASK(HARDIRQ_BITS) << HARDIRQ_SHIFT)
#define NMI_MASK	(__IRQ_MASK(NMI_BITS)     << NMI_SHIFT)

#define PREEMPT_OFFSET	(1UL << PREEMPT_SHIFT)
#define SOFTIRQ_OFFSET	(1UL << SOFTIRQ_SHIFT)
#define HARDIRQ_OFFSET	(1UL << HARDIRQ_SHIFT)
#define NMI_OFFSET	(1UL << NMI_SHIFT)

//TODO
#define hardirq_count()	(preempt_count() & HARDIRQ_MASK)
#define softirq_count()	(preempt_count() & SOFTIRQ_MASK)
#define irq_count()	(preempt_count() & (HARDIRQ_MASK | SOFTIRQ_MASK \
										| NMI_MASK))

#define in_irq()		(hardirq_count())
#define in_softirq()		(softirq_count())
#define in_interrupt()		(irq_count())

//dpc.c
extern void irq_enter(void);
extern void irq_exit(void);

#endif
