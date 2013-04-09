/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 硬件中断计数器处理
*/
#ifndef HAL_HARDIRQ_H
#define HAL_HARDIRQ_H

/* preempt 计数器分段意义
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
#define PREEMPT_OFFSET	(1UL << PREEMPT_SHIFT)
#define SOFTIRQ_OFFSET	(1UL << SOFTIRQ_SHIFT)
#define HARDIRQ_OFFSET	(1UL << HARDIRQ_SHIFT)
#define NMI_OFFSET	(1UL << NMI_SHIFT)

//TODO

#define in_interrupt()	(0)

extern void irq_enter(void);
extern void irq_exit(void);

#endif
