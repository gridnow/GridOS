/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   LuYan
 *   ARM 平台陷阱／异常处理模块
 *
 */
#include <linkage.h>
#include <compiler.h>

#include <ddk/debug.h>

#include <asm/traps.h>

asmlinkage void __div0(void)
{
	printk("Division by zero in kernel.\n");
	while (1);
}

void __init early_trap_init(void *vectors_base)
{
	printk("early_trap_init vector page = %x.\n", vectors_base);
	TODO("");
}
