/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   常用的编译器属性定义
*/
#ifndef COMPILER_H
#define COMPILER_H

#define __attribute_const__		__attribute__((__const__))
#define  noinline			__attribute__ ((noinline))
#define likely(x)			__builtin_expect(!!(x), 1)
#define unlikely(x)			__builtin_expect(!!(x), 0)
#define __noreturn			__attribute__ ((noreturn))
#define __weak				__attribute__((weak))
#define DLLEXPORT			__attribute__ ((visibility ("default")))

/* 变态的类型 */
#define __const const
#endif

