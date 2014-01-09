/*
 * pthread_spin_trylock.c
 *
 * Description:
 * This translation unit implements spin lock primitives.
 */

#include "pthread.h"
#include "implement.h"


int pthread_spin_trylock (pthread_spinlock_t * lock)
{
  
  switch ((long)
	  PTW32_INTERLOCKED_COMPARE_EXCHANGE(lock,
					           PTW32_SPIN_LOCKED,
					           PTW32_SPIN_UNLOCKED))
    {
    case PTW32_SPIN_UNLOCKED:
      return 0;
    case PTW32_SPIN_LOCKED:
      return EBUSY;
    }

  return EINVAL;
}
