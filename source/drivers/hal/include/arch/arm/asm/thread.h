/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   平台线程描述
 */
#ifndef HAL_ARCH_THREAD_H
#define HAL_ARCH_THREAD_H

#include <linkage.h>
#include <compiler.h>
#include <asm/processor.h>

/* Ring0栈底包括当前线程的根指针 */
struct stack_thread_info
{
	void *thread_object;
};
/************************************************************************/
/* ARCH specified                                                       */
/************************************************************************/
#define KT_ARCH_THREAD_CP0_STACK_SIZE (PAGE_SIZE)

struct kt_arch_thread {
	struct thread_struct ctx;
	unsigned long preempt_count;
};

/* 用硬件加速的方法获取到当前的线程 */
static __always_inline void * kt_arch_get_current()
{
	register unsigned long sp asm ("sp");
	struct stack_thread_info *p;
	p = (struct stack_thread_info *)(sp & ~(KT_ARCH_THREAD_CP0_STACK_SIZE - 1));
	return p->thread_object;
}

/* 用硬件加速的方法获取到CPU指针 */
static __always_inline void * kt_arch_get_cpu()
{
	return 0;
}

#define kt_arch_current() ((struct kt_arch_thread *)kt_arch_get_current())

#endif
