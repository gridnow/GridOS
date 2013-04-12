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
#include <asm/processor.h>

#define ARCH_THREAD_ARCH_X86_32_DEFAULT_EFLAGS ((1<<1)/*固定位*/|X86_EFLAGS_ID| X86_EFLAGS_IF | X86_EFLAGS_RF|X86_EFLAGS_IOPL/*最高的IO权限*/)
struct kt_arch_thread {
	struct thread_struct ctx;
	unsigned long preempt_count;
};


/* 用硬件加速的方法获取到当前的线程 */
static __always_inline void * kt_arch_get_current()
{
	void *current;
#if defined(__i386__)
	/* 思想来自wine NtCurrentTeb接口 */
	__asm__ volatile(".byte 0x64\n\tmovl (0),%0" : "=r" (current));
#else
#error "Thread.h not supports the X86-64"
#endif
	/* see km_cpu */
	return current;
}

/* 用硬件加速的方法获取到CPU指针 */
static __always_inline void * kt_arch_get_cpu()
{
	void * cpu;
#if defined(__i386__)
	__asm__ volatile(".byte 0x64\n\tmovl (4),%0" : "=r" (cpu));
#else
#error "Thread.h not supports the X86-64"
#endif
	/* see km_cpu */
	return cpu;
}

#define kt_arch_current() ((struct kt_arch_thread *)kt_arch_get_current())

#endif

