/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   wuxin
 *   atomic
 */

#include <cl_atomic.h>
#include <atomic.h>

void cl_atomic_inc(struct ke_atomic *atomic)
{
	atomic_inc((atomic_t*)atomic);
}

void cl_atomic_dec(struct ke_atomic *atomic)
{
	atomic_dec((atomic_t*)atomic);
}
