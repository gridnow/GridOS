/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   λ
*/
#ifndef COMMON_LIB_BITS_H
#define COMMON_LIB_BITS_H
#include <types.h>

unsigned long cl_find_next_zero_bit(const unsigned long *addr, unsigned long size,
				 unsigned long offset);
#endif

