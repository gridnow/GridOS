// Boilerplate support routines for -*- C++ -*- dynamic memory management.


#include "new"
#include <bits/c++config.h>
#if _GLIBCXX_HOSTED
#include <cstdlib>
#endif

#if _GLIBCXX_HOSTED
using std::free;
#else
// A freestanding C runtime may not provide "free" -- but there is no
// other reasonable way to implement "operator delete".
extern "C" void free(void *);
#endif

_GLIBCXX_WEAK_DEFINITION void
operator delete (void *ptr) throw ()
{
  	if (ptr)
    	free (ptr);
}
