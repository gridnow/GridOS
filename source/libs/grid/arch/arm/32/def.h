#ifndef ARCH_DEF_H
#define ARCH_DEF_H

# ifndef LOCK
#  ifdef UP
#   define LOCK
#  else
#   define LOCK lock
#  endif
# endif

#endif
