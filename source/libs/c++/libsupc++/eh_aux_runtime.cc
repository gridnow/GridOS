// -*- C++ -*- Common throw conditions.

#include "typeinfo"
#include "exception"
#include <cstdlib>
#include "unwind-cxx.h"
#include "exception_defines.h"

extern "C" void
__cxxabiv1::__cxa_bad_cast ()
{
#ifdef __EXCEPTIONS  
  	throw std::bad_cast();
#else
  	std::abort();
#endif
}

extern "C" void
__cxxabiv1::__cxa_bad_typeid ()
{
#ifdef __EXCEPTIONS  
  	throw std::bad_typeid();
#else
  	std::abort();
#endif
}

