#ifndef ARCH_THREAD_H
#define ARCH_THREAD_H

#include <types.h>
#include <asm/cpu-info.h>
#include <asm/thread.h>


/************************************************************************/
/* ARCH specified                                                       */
/************************************************************************/

#define KT_ARCH_THREAD_CP0_STACK_SIZE (PAGE_SIZE * 4)

struct ko_thread;
static __always_inline unsigned long kt_arch_get_sp0(struct ko_thread * who)
{
	struct kmt_arch_thread * p = (struct kmt_arch_thread*)who;
	return p->sp0;
}
static __always_inline void kt_arch_set_sp0(struct ko_thread * who, unsigned long sp0)
{
	struct kmt_arch_thread * p = (struct kmt_arch_thread*)who;
	p->sp0 = sp0;
}

#endif
