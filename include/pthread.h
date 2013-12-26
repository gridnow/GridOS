/**
*  @defgroup crt_pthread
*  @ingroup 标准C库编程接口
*
*  This is an implementation of the threads API of POSIX 1003.1-2001.
*
*  @{
*/

#ifndef POSIX_THREAD_H
#define POSIX_THREAD_H
#include <types.h>
#include <errno.h>
#include <compiler.h>

BEGIN_C_DECLS;

#define PTCDECL 
#define PTEXPORT DLLEXPORT
/*
 * POSIX Options
 */
#undef _POSIX_THREADS
#define _POSIX_THREADS 200809L

#undef _POSIX_READER_WRITER_LOCKS
#define _POSIX_READER_WRITER_LOCKS 200809L

#undef _POSIX_SPIN_LOCKS
#define _POSIX_SPIN_LOCKS 200809L

#undef _POSIX_BARRIERS
#define _POSIX_BARRIERS 200809L

#undef _POSIX_THREAD_SAFE_FUNCTIONS
#define _POSIX_THREAD_SAFE_FUNCTIONS 200809L

#undef _POSIX_THREAD_ATTR_STACKSIZE
#define _POSIX_THREAD_ATTR_STACKSIZE 200809L

/*
 * The following options are not supported
 */
#undef _POSIX_THREAD_ATTR_STACKADDR
#define _POSIX_THREAD_ATTR_STACKADDR -1

#undef _POSIX_THREAD_PRIO_INHERIT
#define _POSIX_THREAD_PRIO_INHERIT -1

#undef _POSIX_THREAD_PRIO_PROTECT
#define _POSIX_THREAD_PRIO_PROTECT -1

/* TPS is not fully supported.  */
#undef _POSIX_THREAD_PRIORITY_SCHEDULING
#define _POSIX_THREAD_PRIORITY_SCHEDULING -1

#undef _POSIX_THREAD_PROCESS_SHARED
#define _POSIX_THREAD_PROCESS_SHARED -1


/*
 * POSIX 1003.1-2001 Limits
 * ===========================
 *
 * These limits are normally set in <limits.h>, which is not provided with
 * pthreads-win32.
 *
 * PTHREAD_DESTRUCTOR_ITERATIONS
 *                      Maximum number of attempts to destroy
 *                      a thread's thread-specific data on
 *                      termination (must be at least 4)
 *
 * PTHREAD_KEYS_MAX
 *                      Maximum number of thread-specific data keys
 *                      available per process (must be at least 128)
 *
 * PTHREAD_STACK_MIN
 *                      Minimum supported stack size for a thread
 *
 * PTHREAD_THREADS_MAX
 *                      Maximum number of threads supported per
 *                      process (must be at least 64).
 *
 * SEM_NSEMS_MAX
 *                      The maximum number of semaphores a process can have.
 *                      (must be at least 256)
 *
 * SEM_VALUE_MAX
 *                      The maximum value a semaphore can have.
 *                      (must be at least 32767)
 *
 */
#undef _POSIX_THREAD_DESTRUCTOR_ITERATIONS
#define _POSIX_THREAD_DESTRUCTOR_ITERATIONS     4

#undef PTHREAD_DESTRUCTOR_ITERATIONS
#define PTHREAD_DESTRUCTOR_ITERATIONS           _POSIX_THREAD_DESTRUCTOR_ITERATIONS

#undef _POSIX_THREAD_KEYS_MAX
#define _POSIX_THREAD_KEYS_MAX                  128

#undef PTHREAD_KEYS_MAX
#define PTHREAD_KEYS_MAX                        _POSIX_THREAD_KEYS_MAX

#undef PTHREAD_STACK_MIN
#define PTHREAD_STACK_MIN                       0

#undef _POSIX_THREAD_THREADS_MAX
#define _POSIX_THREAD_THREADS_MAX               64

  /* Arbitrary value */
#undef PTHREAD_THREADS_MAX
#define PTHREAD_THREADS_MAX                     2019

#undef _POSIX_SEM_NSEMS_MAX
#define _POSIX_SEM_NSEMS_MAX                    256

  /* Arbitrary value */
#undef SEM_NSEMS_MAX
#define SEM_NSEMS_MAX                           1024

#undef _POSIX_SEM_VALUE_MAX
#define _POSIX_SEM_VALUE_MAX                    32767

#undef SEM_VALUE_MAX
#define SEM_VALUE_MAX                           INT_MAX

typedef unsigned long int pthread_t;
typedef struct pthread_attr_t_ * pthread_attr_t;
typedef struct pthread_once_t_ pthread_once_t;
typedef unsigned int pthread_key_t;
typedef struct pthread_mutex_t_ * pthread_mutex_t;
typedef struct pthread_mutexattr_t_ * pthread_mutexattr_t;
typedef struct pthread_cond_t_ * pthread_cond_t;
typedef struct pthread_condattr_t_ * pthread_condattr_t;
typedef struct pthread_rwlock_t_ * pthread_rwlock_t;
typedef struct pthread_rwlockattr_t_ * pthread_rwlockattr_t;
typedef volatile int pthread_spinlock_t;
typedef struct pthread_barrier_t_ * pthread_barrier_t;
typedef struct pthread_barrierattr_t_ * pthread_barrierattr_t;

/************************************************************************/
/* POSIX Threads                                                        */
/************************************************************************/

enum {
/*
 * pthread_attr_{get,set}detachstate
 */
  PTHREAD_CREATE_JOINABLE       = 0,  /* Default */
  PTHREAD_CREATE_DETACHED       = 1,

/*
 * pthread_attr_{get,set}inheritsched
 */
  PTHREAD_INHERIT_SCHED         = 0,
  PTHREAD_EXPLICIT_SCHED        = 1,  /* Default */

/*
 * pthread_{get,set}scope
 */
  PTHREAD_SCOPE_PROCESS         = 0,
  PTHREAD_SCOPE_SYSTEM          = 1,  /* Default */

/*
 * pthread_setcancelstate paramters
 */
  PTHREAD_CANCEL_ENABLE         = 0,  /* Default */
  PTHREAD_CANCEL_DISABLE        = 1,

/*
 * pthread_setcanceltype parameters
 */
  PTHREAD_CANCEL_ASYNCHRONOUS   = 0,
  PTHREAD_CANCEL_DEFERRED       = 1,  /* Default */

/*
 * pthread_mutexattr_{get,set}pshared
 * pthread_condattr_{get,set}pshared
 */
  PTHREAD_PROCESS_PRIVATE       = 0,
  PTHREAD_PROCESS_SHARED        = 1,

/*
 * pthread_mutexattr_{get,set}robust
 */
  PTHREAD_MUTEX_STALLED         = 0,  /* Default */
  PTHREAD_MUTEX_ROBUST          = 1,

/*
 * pthread_barrier_wait
 */
  PTHREAD_BARRIER_SERIAL_THREAD = -1
};

/************************************************************************/
/* Cancelation                                                          */
/************************************************************************/
#define PTHREAD_CANCELED       ((void *)(size_t) -1)


/************************************************************************/
/* Once Key                                                             */
/************************************************************************/
#define PTHREAD_ONCE_INIT       { PTW32_FALSE, 0, 0, 0}

struct pthread_once_t_
{
  int          done;        /* indicates if user function has been executed */
  void *       lock;
  int          reserved1;
  int          reserved2;
};

/************************************************************************/
/* Object initializers                                                  */
/************************************************************************/
 
#define PTHREAD_MUTEX_INITIALIZER ((pthread_mutex_t)(size_t) -1)
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER ((pthread_mutex_t)(size_t) -2)
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER ((pthread_mutex_t)(size_t) -3)

/*
 * Compatibility with LinuxThreads
 */
#define PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP PTHREAD_RECURSIVE_MUTEX_INITIALIZER
#define PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP PTHREAD_ERRORCHECK_MUTEX_INITIALIZER

#define PTHREAD_COND_INITIALIZER ((pthread_cond_t)(size_t) -1)

#define PTHREAD_RWLOCK_INITIALIZER ((pthread_rwlock_t)(size_t) -1)

#define PTHREAD_SPINLOCK_INITIALIZER ((pthread_spinlock_t)(size_t) -1)


/************************************************************************/
/* Thread                                                               */
/************************************************************************/

/**
	@brief Create a thread

	@return
		0               successfully created thread,
		EINVAL          attr invalid,
		EAGAIN          insufficient resources.
*/
PTEXPORT int pthread_create(pthread_t * tid, const pthread_attr_t * attr, void *(PTCDECL *start) (void *), void *arg);

/*
 * Spinlock Functions
 */
PTEXPORT int PTCDECL pthread_spin_init (pthread_spinlock_t * lock, int pshared);

PTEXPORT int PTCDECL pthread_spin_destroy (pthread_spinlock_t * lock);

PTEXPORT int PTCDECL pthread_spin_lock (pthread_spinlock_t * lock);

PTEXPORT int PTCDECL pthread_spin_trylock (pthread_spinlock_t * lock);

PTEXPORT int PTCDECL pthread_spin_unlock (pthread_spinlock_t * lock);

/*
 * KEY
 */
PTEXPORT void *pthread_getspecific (pthread_key_t key);

END_C_DECLS;

#endif

/** @} */
