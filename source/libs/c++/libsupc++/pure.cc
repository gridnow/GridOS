// -*- C++ -*- 

#include <bits/c++config.h>
#include <cxxabi.h>
#include "unwind-cxx.h"

#if _GLIBCXX_HOSTED
#ifdef _GLIBCXX_HAVE_UNISTD_H
# include <unistd.h>
# define writestr(str)	write(2, str, sizeof(str) - 1)
# ifdef __GNU_LIBRARY__
  /* Avoid forcing the library's meaning of `write' on the user program
     by using the "internal" name (for use within the library).  */
/*#  define write(fd, buf, n)	__write((fd), (buf), (n))*/
# endif
#else
# include <cstdio>
# define writestr(str)	std::fputs(str, stderr)
#endif
#else
# define writestr(str) /* Empty */
#endif

extern "C" void
__cxxabiv1::__cxa_pure_virtual (void)
{
  	writestr ("pure virtual method called\n");
  	std::terminate ();
}
