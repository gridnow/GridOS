/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL IRQ FLAGS
*/

#ifndef HAL_IRQ_FLAGS_H
#define HAL_IRQ_FLAGS_H

#include <types.h>
#include <asm/irqflags.h>

#define raw_local_irq_save(flags)			\
	do {						\
	typecheck(unsigned long, flags);	\
	flags = arch_local_irq_save();		\
	} while (0)
#define raw_local_irq_restore(flags)			\
	do {						\
	typecheck(unsigned long, flags);	\
	arch_local_irq_restore(flags);		\
	} while (0)

#define local_irq_save(flags) raw_local_irq_save(flags)
#define local_irq_restore(flags) raw_local_irq_restore(flags);
#define local_irq_enable arch_local_irq_enable
#define local_irq_disable arch_local_irq_disable

#define raw_local_save_flags(flags)			\
	do {						\
		typecheck(unsigned long, flags);	\
		flags = arch_local_save_flags();	\
	} while (0)

#endif /* HAL_IRQ_FLAGS_H */

