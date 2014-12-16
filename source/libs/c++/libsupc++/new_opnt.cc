// Support routines for the -*- C++ -*- dynamic memory management.


#include "new"
#include <exception_defines.h>
#include <bits/c++config.h>

using std::new_handler;
using std::bad_alloc;

extern "C" void *malloc (std::size_t);
extern new_handler __new_handler;

_GLIBCXX_WEAK_DEFINITION void *
operator new (std::size_t sz, const std::nothrow_t&) throw()
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
			return 0;
      	try
		{
	  		handler ();
		}
      	catch (bad_alloc &)
		{
	  		return 0;
		}

      	p = (void *) malloc (sz);
    }

  	return p;
}
