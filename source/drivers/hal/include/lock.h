/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL原子锁
*/

#ifndef HAL_ATOMIC_LOCK_H
#define HAL_ATOMIC_LOCK_H

#include <asm/spinlock.h>
#include <irqflags.h>

struct ke_spinlock
{
	arch_spinlock_t lock;
};

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

#define __KE_SPIN_LOCK_UNLOCKED(lockname)	\
	(struct ke_spinlock) __RAW_SPIN_LOCK_INITIALIZER(lockname)
# define __spin_lock_init(lock)				\
	do { *(lock) = __KE_SPIN_LOCK_UNLOCKED(lock); } while (0)

STATIC inline void ke_spin_init(struct ke_spinlock * lock)
{
	__spin_lock_init(lock);
}

STATIC inline void ke_spin_lock(struct ke_spinlock * lock)
{
	arch_spin_lock((arch_spinlock_t*)lock);
}

STATIC inline void ke_spin_unlock(struct ke_spinlock * lock)
{
	arch_spin_unlock((arch_spinlock_t*)lock);
}

STATIC inline bool ke_spin_lock_try(struct ke_spinlock * lock)
{
	arch_spin_trylock((arch_spinlock_t*)lock);
}

/**
	@brief 自旋锁解锁，并回复IRQ状态
*/
STATIC inline void ke_spin_unlock_irqrestore(struct ke_spinlock * lock, unsigned long flags)
{
	_raw_spin_unlock_irqrestore((arch_spinlock_t*)lock, flags);
}
 
/**
	@brief 自旋锁加锁，关闭IRQ，并返回关闭IRQ前的状态
*/
STATIC inline unsigned long ke_spin_lock_irqsave(struct ke_spinlock * lock)
{
	unsigned long flags;
	_raw_spin_lock_irqsave((arch_spinlock_t*)lock, flags);
	return flags;
}

STATIC inline void ke_spin_lock_irq(struct ke_spinlock * lock)
{
	local_irq_disable();
	ke_spin_lock(lock);
}

STATIC inline void ke_spin_unlock_irq(struct ke_spinlock * lock)
{
	ke_spin_unlock(lock);
	local_irq_enable();
}

#endif
