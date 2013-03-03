/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   编译器属性定义
*/
#ifndef HAL_COMPILER_H
#define HAL_COMPILER_H

/* Compiler macro */
#define __cold				__attribute__((__cold__))					//Gcc 认为函数调用是unlikely
 
#define __force
#define __iomem
#define __kernel
#define __percpu
#define __used				__attribute__((__used__))
#define __pure				__attribute__((pure))
#define __weak				__attribute__((weak))
#define __aligned(x)		__attribute__((aligned(x)))
#define notrace				__attribute__((no_instrument_function))
#define __always_inline		inline __attribute__((always_inline))
#define barrier()			__asm__ __volatile__("": : :"memory")
#define __maybe_unused		__attribute__((unused))

#define __bitwise
#define RELOC_HIDE(ptr, off)					\
	({ unsigned long __ptr;					\
	__asm__ ("" : "=r"(__ptr) : "0"(ptr));		\
	(typeof(ptr)) (__ptr + (off)); })


/* Simple shorthand for a section definition */
#ifndef __section
# define __section(S)		 __attribute__ ((__section__(#S)))
#endif

#define __init				__section(.init.text) __cold notrace
#define __initdata			__section(.init.data)
#define __initconst			__section(.init.rodata)
#define __exitdata			__section(.exit.data)
#define __exit_call			__used __section(.exitcall.exit)
#define __devinit        __section(.devinit.text) __cold notrace
#define __devinitconst   __section(.devinit.rodata)
#define __cpuinit        __section(.cpuinit.text) __cold
#define __cpuinitdata    __section(.cpuinit.data)
#define __cpuinitconst   __section(.cpuinit.rodata)
#define __cpuexit        __section(.cpuexit.text) __exitused __cold
#define __cpuexitdata    __section(.cpuexit.data)
#define __cpuexitconst   __section(.cpuexit.rodata)
#define __page_aligned_data	__section(.data..page_aligned) __aligned(PAGE_SIZE)
#define __read_mostly	__attribute__((__section__(".data..read_mostly")))
#define __refdata        __section(.ref.data)

/* Check at compile time that something is of a particular type. */
#define typecheck(type,x) \
({	type __dummy; \
	typeof(x) __dummy2; \
	(void)(&__dummy == &__dummy2); \
	1; \
})

#define ACCESS_ONCE(x) (*(volatile typeof(x) *)&(x))

#include_next <compiler.h>
#endif
