// Boilerplate support routines for -*- C++ -*- dynamic memory management.

#include "new"
#include <bits/c++config.h>
 
_GLIBCXX_WEAK_DEFINITION void*
operator new[] (std::size_t sz, const std::nothrow_t& nothrow) throw()
{
  	return ::operator new(sz, nothrow);
}
