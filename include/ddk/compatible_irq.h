/*
	OLD driver need this
*/
#ifndef DDK_COMPATIBLE_INTERFACE_IRQ
#define DDK_COMPATIBLE_INTERFACE_IRQ

#include <ddk/irq.h>

/* IRQ */
#define local_irq_save hal_local_irq_save
#define local_irq_restore hal_local_irq_restore
#endif

