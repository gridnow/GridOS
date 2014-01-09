/**
 *  @defgroup STDLIB
 *  @ingroup 标准C库编程接口
 *
 *  标准库操作接口，头文件是stdlib.h
 *
 *  @{
 */
#ifndef ALLOCA_H
#define ALLOCA_H

#undef alloca

#ifdef	__GNUC__
# define alloca(size)	__builtin_alloca (size)
#endif /* GCC.  */

#endif

/** @} */
