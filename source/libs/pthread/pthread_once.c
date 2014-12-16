/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

#include <pthread.h>

/**
	@brief it call init_routine only one base on once_control struct.
*/
int pthread_once(pthread_once_t * once_control, void(* init_routine)(void))
{
	/* it good first test init_routine has exec? */
	if (!(once_control->done))
	{
		/* here, we must hold mutex. */
		pthread_spin_lock(&once_control->lock);
		if (!(once_control->done))
		{
			init_routine();
			/* TODO: where is need prevent write before read ? */
			once_control->done = 1;
		}
		pthread_spin_unlock(&once_control->lock);
	}

	return 0;
}



