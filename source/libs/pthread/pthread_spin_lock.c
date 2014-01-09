#include <pthread.h>

#include "implement.h"

int pthread_spin_lock (pthread_spinlock_t * lock)
{
	while (PTW32_SPIN_LOCKED ==
		PTW32_INTERLOCKED_COMPARE_EXCHANGE(lock,
		PTW32_SPIN_LOCKED,
		PTW32_SPIN_UNLOCKED))
	{
		/* going into wait status */

	}

	return 0;
}
