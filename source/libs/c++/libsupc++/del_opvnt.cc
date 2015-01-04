// Boilerplate support routines for -*- C++ -*- dynamic memory management.


#include "new"
#include <bits/c++config.h>

_GLIBCXX_WEAK_DEFINITION void
operator delete[] (void *ptr, const std::nothrow_t&) throw ()
{
  	::operator delete (ptr);
}
