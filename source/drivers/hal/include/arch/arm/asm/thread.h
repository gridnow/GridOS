/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ƽ̨�߳�����
 */
#ifndef HAL_ARCH_THREAD_H
#define HAL_ARCH_THREAD_H

#include <linkage.h>
#include <compiler.h>
#include <asm/processor.h>

/* Ring0ջ�װ�����ǰ�̵߳ĸ�ָ�� */
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

/* ��Ӳ�����ٵķ�����ȡ����ǰ���߳� */
static __always_inline void * kt_arch_get_current()
{
	register unsigned long sp asm ("sp");
	struct stack_thread_info *p;
	p = (struct stack_thread_info *)(sp & ~(KT_ARCH_THREAD_CP0_STACK_SIZE - 1));
	return p->thread_object;
}

/* ��Ӳ�����ٵķ�����ȡ��CPUָ�� */
static __always_inline void * kt_arch_get_cpu()
{
	return 0;
}

#define kt_arch_current() ((struct kt_arch_thread *)kt_arch_get_current())

#endif
