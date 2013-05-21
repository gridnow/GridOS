/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup stdarg
*  @ingroup 标准C库编程接口
*
*  标准ARG
*
*  @{
*/
#ifndef STDARG_H
#define STDARG_H

#if defined (__mips__)
typedef void * va_list;
#else
/* i386 mode */
typedef char * va_list;
#endif

#define _INTSIZEOF(n)		( (sizeof(n) + sizeof(unsigned long) - 1) & ~(sizeof(unsigned long) - 1) )
#define va_arg(ap,t)		( *(t *)(((ap) += _INTSIZEOF(t)) - _INTSIZEOF(t)) )
#define va_copy(d, s)		(d = s)

#if defined (__arm__)
#define va_start(ap,v)		(ap = (va_list)&v + _INTSIZEOF(v))
#define va_end(ap)			(ap = (va_list)0)
#else
#define va_start(v,l)		__builtin_va_start(v,l)
#define va_end(v)			__builtin_va_end(v)
#endif

#endif

/** @} */

