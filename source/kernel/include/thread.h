/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   线程描述
*/

#ifndef KT_THREAD_H
#define KT_THREAD_H

#include <arch/thread.h>
#include <list.h>
#include <lock.h>

struct ko_process;
struct ko_thread
{
	struct kt_arch_thread arch_thread;
	struct ko_process *process;
	struct ke_spinlock ops_lock;

	struct list_head queue_list;
	unsigned long state;
	unsigned char priority_level;
	unsigned int wakeup_count;
};

/* Context for thread creating */
struct kt_thread_creating_context
{
	unsigned long stack_pos;
	unsigned long stack0, stack0_size;
	unsigned long fate_entry;
	unsigned long thread_entry;
	int priority;
	unsigned long para;
	int cpl;
};

#define KT_CREATE_STACK_AS_PARA			(1 << 1)
#define KT_STATE_RUNNING				1
#define KT_STATE_MASK					(0xffff)
#define KT_STATE_ADD_UNINTERRUPT		16												//附加属性：不可中断地做某件事情
#define KT_STATE_ADD_FORCE_BY_SYSTEM	17												//附加属性：强制休眠的，常规唤醒（用户层的唤醒）是不能的
#define KT_STATE_ADD_DIEING				18												//附加属性：要死了...
#define KT_STATE_ADD_USING_FPU			19												//附加属性：线程本次试用了FPU.

//thread.c
void kt_wakeup(struct ko_thread *who);

//arch
void kt_arch_switch(struct ko_thread *prev, struct ko_thread *next);
void kt_arch_init_thread(struct ko_thread * thread, struct kt_thread_creating_context * ctx);

#endif

