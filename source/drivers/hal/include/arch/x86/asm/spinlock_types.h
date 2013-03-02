#ifndef _ASM_X86_SPINLOCK_TYPES_H
#define _ASM_X86_SPINLOCK_TYPES_H

typedef struct arch_spinlock {
	unsigned int slock;
} arch_spinlock_t;

#define __ARCH_SPIN_LOCK_UNLOCKED	{ 0 }

#define __RAW_SPIN_LOCK_INITIALIZER	\
{					\
	.slock = __ARCH_SPIN_LOCK_UNLOCKED  \
}

#include <asm/rwlock.h>


#endif /* _ASM_X86_SPINLOCK_TYPES_H */
