// -*- C++ -*- Manage the thread-local exception globals.

#include <exception>
#include <cstdlib>
#include "unwind-cxx.h"
#include "bits/c++config.h"
#include "gthr.h"

using namespace __cxxabiv1;


// Single-threaded fallback buffer.
static __cxa_eh_globals globals_static;

#if __GTHREADS
static __gthread_key_t globals_key;
static int use_thread_key = -1;

static void
get_globals_dtor (void *ptr)
{
  	if (ptr)
    {
      	__cxa_exception *exn, *next;
      	exn = ((__cxa_eh_globals *) ptr)->caughtExceptions;
      	while (exn)
		{
	  		next = exn->nextException;
	  		_Unwind_DeleteException (&exn->unwindHeader);
	  		exn = next;
		}
      	std::free (ptr);
    }
}

static void
get_globals_init ()
{
  	use_thread_key =
    	(__gthread_key_create (&globals_key, get_globals_dtor) == 0);
}

static void
get_globals_init_once ()
{
  	static __gthread_once_t once = __GTHREAD_ONCE_INIT;
  	if (__gthread_once (&once, get_globals_init) != 0
      	|| use_thread_key < 0)
    	use_thread_key = 0;
}
#endif

extern "C" __cxa_eh_globals *
__cxxabiv1::__cxa_get_globals_fast () throw()
{
#if __GTHREADS
  	if (use_thread_key)
    	return (__cxa_eh_globals *) __gthread_getspecific (globals_key);
  	else
    	return &globals_static;
#else
  	return &globals_static;
#endif
}

extern "C" __cxa_eh_globals *
__cxxabiv1::__cxa_get_globals () throw()
{
#if __GTHREADS
  	__cxa_eh_globals *g;

  	if (use_thread_key == 0)
    	return &globals_static;

  	if (use_thread_key < 0)
    {
      	get_globals_init_once ();

      	// Make sure use_thread_key got initialized.
      	if (use_thread_key == 0)
			return &globals_static;
    }

  	g = (__cxa_eh_globals *) __gthread_getspecific (globals_key);
  	if (! g)
    {
      	if ((g = (__cxa_eh_globals *)
	   		std::malloc (sizeof (__cxa_eh_globals))) == 0
	  		|| __gthread_setspecific (globals_key, (void *) g) != 0)
        		std::terminate ();
      		g->caughtExceptions = 0;
      		g->uncaughtExceptions = 0;
    }

  	return g;
#else
  	return &globals_static;
#endif
}
