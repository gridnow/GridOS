// -*- C++ -*- std::terminate, std::unexpected and friends.

#include "typeinfo"
#include "exception"
#include <cstdlib>
#include "unwind-cxx.h"
#include "exception_defines.h"

using namespace __cxxabiv1;

void
__cxxabiv1::__terminate (std::terminate_handler handler)
{
  	try {
    	handler ();
    	std::abort ();
  	} catch (...) {
    	std::abort ();
  	}
}

void
std::terminate ()
{
  	__terminate (__terminate_handler);
}

void
__cxxabiv1::__unexpected (std::unexpected_handler handler)
{
  	handler();
  	std::terminate ();
}

void
std::unexpected ()
{
  	__unexpected (__unexpected_handler);
}

std::terminate_handler
std::set_terminate (std::terminate_handler func) throw()
{
  	std::terminate_handler old = __terminate_handler;
  	__terminate_handler = func;
  	return old;
}

std::unexpected_handler
std::set_unexpected (std::unexpected_handler func) throw()
{
  	std::unexpected_handler old = __unexpected_handler;
  	__unexpected_handler = func;
  	return old;
}
