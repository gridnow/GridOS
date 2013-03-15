/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   ���õı��������Զ���
*/
#ifndef COMPILER_H
#define COMPILER_H

#define  noinline			__attribute__ ((noinline))
#define likely(x)			__builtin_expect(!!(x), 1)
#define unlikely(x)			__builtin_expect(!!(x), 0)
#define __noreturn			__attribute__ ((noreturn))
#define DLLEXPORT			__attribute__ ((visibility ("default")))

#endif

