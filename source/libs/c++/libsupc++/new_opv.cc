// Boilerplate support routines for -*- C++ -*- dynamic memory management.

#include "new"
#include <bits/c++config.h>

_GLIBCXX_WEAK_DEFINITION void*
operator new[] (std::size_t sz) throw (std::bad_alloc)
{
  	return ::operator new(sz);
}
