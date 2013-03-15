#ifndef _ASM_X86_SPINLOCK_H
#define _ASM_X86_SPINLOCK_H

#include <compiler.h>
#include <atomic.h>

#include "spinlock_types.h"
#include "cmpxchg.h"

#ifdef CONFIG_X86_32
# define LOCK_PTR_REG "a"
# define REG_PTR_MODE "k"
#else
# define LOCK_PTR_REG "D"
# define REG_PTR_MODE "q"
#endif

#if defined(CONFIG_X86_32) && \
	(defined(CONFIG_X86_OOSTORE) || defined(CONFIG_X86_PPRO_FENCE))
/*
* On PPro SMP or if we are using OOSTORE, we use a locked operation to unlock
* (PPro errata 66, 92)
*/
# define UNLOCK_LOCK_PREFIX LOCK_PREFIX
#else
# define UNLOCK_LOCK_PREFIX
#endif

static __always_inline void __ticket_spin_lock(arch_spinlock_t *lock)
{
	register struct __raw_tickets inc = { .tail = 1 };

	inc = xadd(&lock->tickets, inc);

	for (;;) {
		if (inc.head == inc.tail)
			break;
		cpu_relax();
		inc.head = ACCESS_ONCE(lock->tickets.head);
	}
	barrier();		/* make sure nothing creeps before the lock is taken */
}

static __always_inline int __ticket_spin_trylock(arch_spinlock_t *lock)
{
	arch_spinlock_t old, new;

	old.tickets = ACCESS_ONCE(lock->tickets);
	if (old.tickets.head != old.tickets.tail)
		return 0;

	new.head_tail = old.head_tail + (1 << TICKET_SHIFT);

	/* cmpxchg is a full barrier, so nothing can move before it */
	return cmpxchg(&lock->head_tail, old.head_tail, new.head_tail) == old.head_tail;
}

static __always_inline void __ticket_spin_unlock(arch_spinlock_t *lock)
{
	__add(&lock->tickets.head, 1, UNLOCK_LOCK_PREFIX);
}

static inline int __ticket_spin_is_locked(arch_spinlock_t *lock)
{
	struct __raw_tickets tmp = ACCESS_ONCE(lock->tickets);

	return tmp.tail != tmp.head;
}

static inline int __ticket_spin_is_contended(arch_spinlock_t *lock)
{
	struct __raw_tickets tmp = ACCESS_ONCE(lock->tickets);

	return (__ticket_t)(tmp.tail - tmp.head) > 1;
}

/* Usable version */
static inline int arch_spin_is_locked(arch_spinlock_t *lock)
{
	return __ticket_spin_is_locked(lock);
}

static inline int arch_spin_is_contended(arch_spinlock_t *lock)
{
	return __ticket_spin_is_contended(lock);
}
#define arch_spin_is_contended	arch_spin_is_contended

static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
{
	__ticket_spin_lock(lock);
}

static __always_inline int arch_spin_trylock(arch_spinlock_t *lock)
{
	return __ticket_spin_trylock(lock);
}

static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	__ticket_spin_unlock(lock);
}

static __always_inline void arch_spin_lock_flags(arch_spinlock_t *lock,
						  unsigned long flags)
{
	arch_spin_lock(lock);
}

/* */
static inline int arch_write_can_lock(arch_rwlock_t *lock)
{
	return lock->write == WRITE_LOCK_CMP;
}

static inline void arch_read_lock(arch_rwlock_t *rw)
{
	asm volatile(LOCK_PREFIX READ_LOCK_SIZE(dec) " (%0)\n\t"
		     "jns 1f\n"
		     "call __read_lock_failed\n\t"
		     "1:\n"
		     ::LOCK_PTR_REG (rw) : "memory");
}

static inline void arch_write_lock(arch_rwlock_t *rw)
{
	asm volatile(LOCK_PREFIX WRITE_LOCK_SUB(%1) "(%0)\n\t"
		     "jz 1f\n"
		     "call __write_lock_failed\n\t"
		     "1:\n"
		     ::LOCK_PTR_REG (&rw->write), "i" (RW_LOCK_BIAS)
		     : "memory");
}

static inline int arch_read_trylock(arch_rwlock_t *lock)
{
	READ_LOCK_ATOMIC(t) *count = (READ_LOCK_ATOMIC(t) *)lock;

	if (READ_LOCK_ATOMIC(dec_return)(count) >= 0)
		return 1;
	READ_LOCK_ATOMIC(inc)(count);
	return 0;
}

static inline int arch_write_trylock(arch_rwlock_t *lock)
{
	atomic_t *count = (atomic_t *)&lock->write;

	if (atomic_sub_and_test(WRITE_LOCK_CMP, count))
		return 1;
	atomic_add(WRITE_LOCK_CMP, count);
	return 0;
}

static inline void arch_read_unlock(arch_rwlock_t *rw)
{
	asm volatile(LOCK_PREFIX READ_LOCK_SIZE(inc) " %0"
		     :"+m" (rw->lock) : : "memory");
}

static inline void arch_write_unlock(arch_rwlock_t *rw)
{
	asm volatile(LOCK_PREFIX WRITE_LOCK_ADD(%1) "%0"
		     : "+m" (rw->write) : "i" (RW_LOCK_BIAS) : "memory");
}

#define arch_read_lock_flags(lock, flags) arch_read_lock(lock)
#define arch_write_lock_flags(lock, flags) arch_write_lock(lock)

#undef READ_LOCK_SIZE
#undef READ_LOCK_ATOMIC
#undef WRITE_LOCK_ADD
#undef WRITE_LOCK_SUB
#undef WRITE_LOCK_CMP

#define arch_spin_relax(lock)	cpu_relax()
#define arch_read_relax(lock)	cpu_relax()
#define arch_write_relax(lock)	cpu_relax()

/* The {read|write|spin}_lock() on x86 are full memory barriers. */
static inline void smp_mb__after_lock(void) { }
#define ARCH_HAS_SMP_MB_AFTER_LOCK

#endif

