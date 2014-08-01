#ifndef __ASM_HW_IRQ_H
#define __ASM_HW_IRQ_H

#include <hal_atomic.h>

extern atomic_t irq_err_count;

/*
 * interrupt-retrigger: NOP for now. This may not be appropriate for all
 * machines, we'll see ...
 */

#endif /* __ASM_HW_IRQ_H */
