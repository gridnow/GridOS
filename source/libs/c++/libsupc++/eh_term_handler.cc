// -*- C++ -*- std::terminate handler

#include "unwind-cxx.h"
#include <bits/c++config.h>

/* We default to the talkative, informative handler in a normal hosted
   library.  This pulls in the demangler, the dyn-string utilities, and
   elements of the I/O library.  For a low-memory environment, you can return
   to the earlier "silent death" handler by including <cstdlib>, initializing
   to "std::abort", and rebuilding the library.  In a freestanding mode, we
   default to this latter approach.  */

#if ! _GLIBCXX_HOSTED
# include <cstdlib>
#endif

/* The current installed user handler.  */
std::terminate_handler __cxxabiv1::__terminate_handler =
#if _GLIBCXX_HOSTED
	__gnu_cxx::__verbose_terminate_handler;
#else
	std::abort;
#endif

