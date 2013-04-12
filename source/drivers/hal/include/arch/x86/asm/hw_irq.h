#ifndef __ASM_HW_IRQ_H
#define __ASM_HW_IRQ_H

#include <asm/irq_vectors.h>

extern void (*__initconst interrupt[NR_VECTORS-FIRST_EXTERNAL_VECTOR])(void);

typedef int vector_irq_t[NR_VECTORS];
extern vector_irq_t vector_irq;

#endif

