// Copyright (C) 2002 Free Software Foundation, Inc.

// Written by Mark Mitchell, CodeSourcery LLC, <mark@codesourcery.com>
// Thread support written by Jason Merrill, Red Hat Inc. <jason@redhat.com>

#include <cxxabi.h>
#include <exception>
#include <bits/c++config.h>
#include <gthr.h>
#include <bits/atomicity.h>

// The IA64/generic ABI uses the first byte of the guard variable.
// The ARM EABI uses the least significant bit.

// Thread-safe static local initialization support.
#ifdef __GTHREADS
namespace
{
  // static_mutex is a single mutex controlling all static initializations.
  // This is a static class--the need for a static initialization function
  // to pass to __gthread_once precludes creating multiple instances, though
  // I suppose you could achieve the same effect with a template.
  class static_mutex
  {
    	static __gthread_recursive_mutex_t mutex;

#ifdef __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION
    	static void init();
#endif

  	public:
    	static void lock();
    	static void unlock();
  };

  	__gthread_recursive_mutex_t static_mutex::mutex
#ifdef __GTHREAD_RECURSIVE_MUTEX_INIT
  	= __GTHREAD_RECURSIVE_MUTEX_INIT
#endif
  ;

#ifdef __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION
  	void static_mutex::init()
  	{
    	__GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION (&mutex);
  	}
#endif

  	void static_mutex::lock()
  	{
#ifdef __GTHREAD_RECURSIVE_MUTEX_INIT_FUNCTION
    	static __gthread_once_t once = __GTHREAD_ONCE_INIT;
    	__gthread_once (&once, init);
#endif
    	__gthread_recursive_mutex_lock (&mutex);
  	}

  	void static_mutex::unlock ()
  	{
    	__gthread_recursive_mutex_unlock (&mutex);
  	}
}

#ifndef _GLIBCXX_GUARD_TEST_AND_ACQUIRE
inline bool
__test_and_acquire (__cxxabiv1::__guard *g)
{
  	bool b = _GLIBCXX_GUARD_TEST (g);
  	_GLIBCXX_READ_MEM_BARRIER;
  	return b;
}
#define _GLIBCXX_GUARD_TEST_AND_ACQUIRE(G) __test_and_acquire (G)
#endif

#ifndef _GLIBCXX_GUARD_SET_AND_RELEASE
inline void
__set_and_release (__cxxabiv1::__guard *g)
{
  	_GLIBCXX_WRITE_MEM_BARRIER;
  	_GLIBCXX_GUARD_SET (g);
}
#define _GLIBCXX_GUARD_SET_AND_RELEASE(G) __set_and_release (G)
#endif

#else /* !__GTHREADS */

#undef _GLIBCXX_GUARD_TEST_AND_ACQUIRE
#undef _GLIBCXX_GUARD_SET_AND_RELEASE
#define _GLIBCXX_GUARD_SET_AND_RELEASE(G) _GLIBCXX_GUARD_SET (G)

#endif /* __GTHREADS */

namespace __gnu_cxx
{
  	// 6.7[stmt.dcl]/4: If control re-enters the declaration (recursively)
  	// while the object is being initialized, the behavior is undefined.

  	// Since we already have a library function to handle locking, we might
  	// as well check for this situation and throw an exception.
  	// We use the second byte of the guard variable to remember that we're
  	// in the middle of an initialization.
	class recursive_init: public std::exception
  	{
		public:
    		recursive_init() throw() { }
    		virtual ~recursive_init() throw ();
  	};

  	recursive_init::~recursive_init() throw() { }
}

namespace __cxxabiv1 
{
  	static inline int
  	recursion_push (__guard* g)
  	{
    	return ((char *)g)[1]++;
  	}

  	static inline void
  	recursion_pop (__guard* g)
  	{
    	--((char *)g)[1];
  	}

  	static int
  	acquire_1 (__guard *g)
  	{
    	if (_GLIBCXX_GUARD_TEST (g))
      		return 0;

    	if (recursion_push (g))
      	{
#ifdef __EXCEPTIONS
			throw __gnu_cxx::recursive_init();
#else
			// Use __builtin_trap so we don't require abort().
			__builtin_trap ();
#endif
      	}
    	return 1;
  	}

  	extern "C"
  	int __cxa_guard_acquire (__guard *g) 
  	{
#ifdef __GTHREADS
    	// If the target can reorder loads, we need to insert a read memory
    	// barrier so that accesses to the guarded variable happen after the
    	// guard test.
    	if (_GLIBCXX_GUARD_TEST_AND_ACQUIRE (g))
      		return 0;

    	if (__gthread_active_p ())
      	{
			// Simple wrapper for exception safety.
			struct mutex_wrapper
			{
	  			bool unlock;
	  			mutex_wrapper (): unlock(true)
	  			{
	    			static_mutex::lock ();
	  			}
	  			~mutex_wrapper ()
	  			{
	    				if (unlock)
	      					static_mutex::unlock ();
	  			}
			} mw;

		if (acquire_1 (g))
	  	{
	    	mw.unlock = false;
	    	return 1;
	  	}

		return 0;
      }
#endif

    	return acquire_1 (g);
  	}

  	extern "C"
  	void __cxa_guard_abort (__guard *g)
  	{
    	recursion_pop (g);
#ifdef __GTHREADS
    	if (__gthread_active_p ())
      		static_mutex::unlock ();
#endif
  	}

  	extern "C"
  	void __cxa_guard_release (__guard *g)
  	{
    	recursion_pop (g);
    	_GLIBCXX_GUARD_SET_AND_RELEASE (g);
#ifdef __GTHREADS
    	if (__gthread_active_p ())
      		static_mutex::unlock ();
#endif
  	}
}
