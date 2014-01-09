/*
 * pthread_spin_init.c
 *
 * Description:
 * This translation unit implements spin lock primitives.
 */

#include "pthread.h"
#include "implement.h"

int pthread_spin_init (pthread_spinlock_t * lock, int pshared)
{
	* lock = 0;
}
