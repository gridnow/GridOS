/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   HALÔ­×ÓËø
 */

#ifndef HAL_ATOMIC_LOCK_H
#define HAL_ATOMIC_LOCK_H

#include <asm/spinlock.h>
#include <irqflags.h>

#ifndef SPINLOCK_T
typedef struct
{
	arch_spinlock_t lock;
} spinlock_t;
#define SPINLOCK_T
#endif

#ifdef CONFIG_SMP
#define _raw_spin_lock_irqsave(arch_lock, flags)	\
do{												\
	local_irq_save(flags);						\
	arch_spin_lock_flags(arch_lock, flags);		\
}while (0)
#define _raw_spin_unlock_irqrestore(arch_lock, flags)	\
do{													\
	arch_spin_unlock(arch_lock);					\
	local_irq_restore(flags);						\
}while (0)

#else
#error "NON SMP version not finished"
#endif

#define __RAW_SPIN_LOCK_INITIALIZER(lockname)	\
{					\
	.lock = __ARCH_SPIN_LOCK_UNLOCKED,	\
	}
#define __RAW_SPIN_LOCK_UNLOCKED(lockname)	\
	(raw_spinlock_t) __RAW_SPIN_LOCK_INITIALIZER(lockname)
	
#include <ddk/compatible.h>
	
#endif


