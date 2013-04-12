#ifndef DDK_COMPATIBLE_H
#define DDK_COMPATIBLE_H

/* Compiler macro */
#define __cold				__attribute__((__cold__))					//Gcc 认为函数调用是unlikely
 
#define __force
#define __iomem
#define __kernel
#define __percpu
#define __used				__attribute__((__used__))
#define __pure				__attribute__((pure))
#define __read_mostly	__attribute__((__section__(".data..read_mostly")))

# define __section(S)		 __attribute__ ((__section__(#S)))

#define __init				__section(.init.text) __cold
#define __devinit        __section(.devinit.text) __cold
#define __devinitconst   __section(.devinit.rodata)
#define __initdata			__section(.init.data)

/**
	自动调用列表
*/
typedef int (*initcall_t)(void);
#define __define_initcall(level,fn,id) \
	static initcall_t __initcall_##fn##id __used \
	__attribute__((__section__(".initcall" level ".init"))) = fn
#define driver_initcall(fn)		__define_initcall("6",fn,6)
#define define_driver_initcall_head(x)	\
	static initcall_t x __used \
	__attribute__((__section__(".initcall6.init")))

#endif
