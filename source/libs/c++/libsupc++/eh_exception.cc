// -*- C++ -*- std::exception implementation.



#include "typeinfo"
#include "exception"
#include "unwind-cxx.h"

std::exception::~exception() throw() { }

std::bad_exception::~bad_exception() throw() { }

const char* 
std::exception::what() const throw()
{
  	return typeid (*this).name ();
}
