#include <compiler.h>

#ifdef __i386__
# include "string_32.h"
#else
# include "string_64.h"
#endif
