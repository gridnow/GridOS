/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ARM×Ö·û´®²Ù×÷½Ó¿Ú
 */
#ifndef COMMON_LIB_ARM_STRING_H
#define COMMON_LIB_ARM_STRING_H

/*
 * We don't do inline string functions, since the
 * optimised inline asm versions are not small.
 */

#define __HAVE_ARCH_STRRCHR
extern char * strrchr(const char * s, int c);

#define __HAVE_ARCH_STRCHR
extern char * strchr(const char * s, int c);

#define __HAVE_ARCH_MEMCPY
extern void * memcpy(void *, const void *, size_t);

#define __HAVE_ARCH_MEMMOVE
extern void * memmove(void *, const void *, size_t);

#define __HAVE_ARCH_MEMCHR
extern void * memchr(const void *, int, size_t);

#define __HAVE_ARCH_MEMSET
extern void * memset(void *, int, size_t);

extern void __memzero(void *ptr, size_t n);

#define memset(p,v,n)							\
({								\
	void *__p = (p); size_t __n = n;			\
	if ((__n) != 0) {					\
	if (__builtin_constant_p((v)) && (v) == 0)	\
		__memzero((__p),(__n));			\
	else						\
		memset((__p),(v),(__n));		\
	}							\
	(__p);							\
})

#endif

