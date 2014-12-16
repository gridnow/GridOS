// Support routines for the -*- C++ -*- dynamic memory management.


#include "new"
#include <cstdlib>
#include <exception_defines.h>
#include <bits/c++config.h>

using std::new_handler;
using std::bad_alloc;
#if _GLIBCXX_HOSTED
using std::malloc;
#else
// A freestanding C runtime may not provide "malloc" -- but there is no
// other reasonable way to implement "operator new".
extern "C" void *malloc (std::size_t);
#endif

extern new_handler __new_handler;

_GLIBCXX_WEAK_DEFINITION void *
operator new (std::size_t sz) throw (std::bad_alloc)
{
	void *p;

	/* malloc (0) is unpredictable; avoid it.  */
	if (sz == 0)
    	sz = 1;
  	p = (void *) malloc (sz);
  	while (p == 0)
    {
		new_handler handler = __new_handler;
      	if (! handler)
#ifdef __EXCEPTIONS
			throw bad_alloc();
#else
        	std::abort();
#endif
      	handler ();
      	p = (void *) malloc (sz);
    }

  	return p;
}
