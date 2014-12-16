// -*- C++ -*- Exception handling routines for throwing.


#include <bits/c++config.h>
#include "unwind-cxx.h"

using namespace __cxxabiv1;


static void
__gxx_exception_cleanup (_Unwind_Reason_Code code, _Unwind_Exception *exc)
{
  	__cxa_exception *header = __get_exception_header_from_ue (exc);

  	// If we haven't been caught by a foreign handler, then this is
  	// some sort of unwind error.  In that case just die immediately.
  	// _Unwind_DeleteException in the HP-UX IA64 libunwind library
  	//  returns _URC_NO_REASON and not _URC_FOREIGN_EXCEPTION_CAUGHT
  	// like the GCC _Unwind_DeleteException function does.
  	if (code != _URC_FOREIGN_EXCEPTION_CAUGHT && code != _URC_NO_REASON)
    	__terminate (header->terminateHandler);

  	if (header->exceptionDestructor)
    	header->exceptionDestructor (header + 1);

	__cxa_free_exception (header + 1);
}


extern "C" void
__cxxabiv1::__cxa_throw (void *obj, std::type_info *tinfo, 
			 void (*dest) (void *))
{
  	__cxa_exception *header = __get_exception_header_from_obj (obj);
  	header->exceptionType = tinfo;
  	header->exceptionDestructor = dest;
  	header->unexpectedHandler = __unexpected_handler;
  	header->terminateHandler = __terminate_handler;
  	header->unwindHeader.exception_class = __gxx_exception_class;
  	header->unwindHeader.exception_cleanup = __gxx_exception_cleanup;

#ifdef _GLIBCXX_SJLJ_EXCEPTIONS
  	_Unwind_SjLj_RaiseException (&header->unwindHeader);
#else
  	_Unwind_RaiseException (&header->unwindHeader);
#endif

  	// Some sort of unwinding error.  Note that terminate is a handler.
  	__cxa_begin_catch (&header->unwindHeader);
  	std::terminate ();
}

extern "C" void
__cxxabiv1::__cxa_rethrow ()
{
  	__cxa_eh_globals *globals = __cxa_get_globals ();
  	__cxa_exception *header = globals->caughtExceptions;

  	globals->uncaughtExceptions += 1;

  	// Watch for luser rethrowing with no active exception.
  	if (header)
    {
      	// Tell __cxa_end_catch this is a rethrow.
      	if (header->unwindHeader.exception_class != __gxx_exception_class)
			globals->caughtExceptions = 0;
      	else
			header->handlerCount = -header->handlerCount;

#ifdef _GLIBCXX_SJLJ_EXCEPTIONS
      	_Unwind_SjLj_Resume_or_Rethrow (&header->unwindHeader);
#else
#ifdef _LIBUNWIND_STD_ABI
      	_Unwind_RaiseException (&header->unwindHeader);
#else
      	_Unwind_Resume_or_Rethrow (&header->unwindHeader);
#endif
#endif
  
      	// Some sort of unwinding error.  Note that terminate is a handler.
      	__cxa_begin_catch (&header->unwindHeader);
    }
  	std::terminate ();
}
