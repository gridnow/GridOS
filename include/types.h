/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
*  @defgroup BasicTypes
*  @ingroup 标准C库编程接口
*
*  编译器相关设置，常用变量类型定义
*
*  @{
*/
#ifndef TYPES_H
#define TYPES_H

/* C++ needs to know that types and declarations are C, not C++.  */
#ifdef	__cplusplus
# define BEGIN_C_DECLS	extern "C" {
# define END_C_DECLS	}
#else
# define BEGIN_C_DECLS
# define END_C_DECLS
#endif

BEGIN_C_DECLS;

#ifndef __ASSEMBLY__
typedef unsigned char		__u8;								/**< 无符号8位数据，建议不用该版本，仅仅为了兼容一些传统软件 */
typedef unsigned char		u8;									/**< 无符号8位数据 */
typedef   signed char		s8;									/**< 有符号8位数据 */
typedef unsigned short		__u16;								/**< 无符号16位数据，建议不用该版本，仅仅为了兼容一些传统软件 */
typedef unsigned short		u16;								/**< 无符号16位数据 */
typedef unsigned int		__u32;								/**< 无符号32位数据，建议不用该版本，仅仅为了兼容一些传统软件 */
typedef unsigned int		u32;								/**< 无符号32位数据 */
typedef signed   int		s32;								/**< 有符号32位数据 */
typedef unsigned long		size_t;								/**< 一般用以表示一个缓冲区长度，无符号版本 */
typedef long				ssize_t;							/**< 一般用以表示一个缓冲区长度，有符号版本 */

#if defined(__i386__)
typedef unsigned long long	u64;								/**< x86-32平台上的无符号64位数据 */
typedef signed long long s64;									/**< x86-32平台上的有符号64位数据 */
#define BITS_PER_LONG		32									/**< x86-32平台上，一个long型变量是32位 */

#elif defined(__mips__)
	#ifdef __mips64
		typedef unsigned long u64;								/**< MIPS64平台上的无符号64位数据 */
		typedef signed long s64;								/**< MIPS64平台上的有符号64位数据 */
		#define BITS_PER_LONG 64								/**< MIPS64平台上，一个long型变量是64位 */
	#else
		typedef unsigned long long	u64;
		typedef signed long long s64;
		#define BITS_PER_LONG 32
	#endif
#else
#error "平台的数据类型没有定义"
#endif
/* 变态的off_t */
typedef s64			off_t;
typedef s64			loff_t;

/* 位类型 */
#define BITS_PER_BYTE              8
#define BITS_TO_LONGS(nr)          DIV_ROUND_UP(nr, BITS_PER_BYTE * sizeof(long))
#define DIV_ROUND_UP(n,d)          (((n) + (d) - 1) / (d))
#define DECLARE_BITMAP(name,bits) \
	unsigned long name[BITS_TO_LONGS(bits)]

/* 字符类型，整个系统有自己的文字编码方式，因此建议使用xchar/xstring类型来统一编码 */
typedef char xchar;
typedef char * xstring;

/* bool */
#ifndef  __cplusplus
typedef unsigned int bool;
#define true 1
#define false 0
#endif
#endif /* __ASSEMBLY__ */

/* Pointer */
#define NULL 0

/* 变量域 */
#define __in
#define __out
#define __user 

END_C_DECLS;

#endif

/** @} */
