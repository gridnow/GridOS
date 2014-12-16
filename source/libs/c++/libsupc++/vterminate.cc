// Verbose terminate_handler -*- C++ -*-

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.


#include <bits/c++config.h>
#if _GLIBCXX_HOSTED
#include <cstdlib>
#include <exception>
#include <exception_defines.h>
#include <cxxabi.h>
# include <cstdio>

using namespace std;
using namespace abi;

namespace __gnu_cxx
{
  	// A replacement for the standard terminate_handler which prints
  	// more information about the terminating exception (if any) on
  	// stderr.
  	void __verbose_terminate_handler()
  	{
    	static bool terminating;
    	if (terminating)
      	{
			fputs("terminate called recursively\n", stderr);
			abort ();
      	}
    	terminating = true;

    	// Make sure there was an exception; terminate is also called for an
    	// attempt to rethrow when there is no suitable exception.
    	type_info *t = __cxa_current_exception_type();
    	if (t)
      	{
			// Note that "name" is the mangled name.
			char const *name = t->name();
			{
	  			int status = -1;
	  			char *dem = 0;
	  
	  			dem = __cxa_demangle(name, 0, 0, &status);

	  			fputs("terminate called after throwing an instance of '", stderr);
	  			if (status == 0)
	    			fputs(dem, stderr);
	  			else
	    			fputs(name, stderr);
	  			fputs("'\n", stderr);

	  			if (status == 0)
	    			free(dem);
			}

			// If the exception is derived from std::exception, we can
			// give more information.
			try { __throw_exception_again; }
#ifdef __EXCEPTIONS
			catch (exception &exc)
	  		{
	    		char const *w = exc.what();
	    		fputs("  what():  ", stderr);
	    		fputs(w, stderr);
	    		fputs("\n", stderr);
			}
#endif
			catch (...) { }
      	}
    	else
      	fputs("terminate called without an active exception\n", stderr);
    
    	abort();
	}
} // namespace __gnu_cxx
#endif
