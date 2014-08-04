#ifndef _ASM_HARDIRQ_H
#define _ASM_HARDIRQ_H

extern void ack_bad_irq(unsigned int irq);
#define ack_bad_irq ack_bad_irq

#include <asm-generic/hardirq.h>

#endif /* _ASM_HARDIRQ_H */
