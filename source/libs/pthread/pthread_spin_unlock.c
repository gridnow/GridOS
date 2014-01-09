/*
 * pthread_spin_unlock.c
 *
 * Description:
 * This translation unit implements spin lock primitives.
 */

#include "pthread.h"
#include "implement.h"

int pthread_spin_unlock (pthread_spinlock_t * lock)
{
	* lock = 0;

	ARCH_ATOMIC_BARRIER;
}
