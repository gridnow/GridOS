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
#include <spinlock.h>

struct ko_process;
struct ko_thread
{
	struct kt_arch_thread arch_thread;
	struct ko_process *process;
	spinlock_t ops_lock;

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
#define KT_CREATE_RUN					(1 << 2)

#define KT_STATE_RUNNING				1
#define KT_STATE_WAITING_SYNC			2
#define KT_STATE_KILLING				3
#define KT_STATE_MASK					(0xffff)
#define KT_STATE_ADD_UNINTERRUPT		(1 << 16)												//附加属性：不可中断地做某件事情
#define KT_STATE_ADD_FORCE_BY_SYSTEM	(1 << 17)												//附加属性：强制休眠的，常规唤醒（用户层的唤醒）是不能的
#define KT_STATE_ADD_DIEING				(1 << 18)												//附加属性：要死了...
#define KT_STATE_ADD_USING_FPU			(1 << 19)												//附加属性：线程本次使用了FPU.
#define KT_STATE_ADD_NO_SWITCHING		(1 << 20)												//附加属性：线程操作后不切换线程.

#define KT_CURRENT_KILLING() ((kt_current()->state & KT_STATE_MASK) == KT_STATE_KILLING)
static inline struct ko_thread *kt_current()
{
	return kt_arch_get_current();
}

//thread.c
struct ko_thread *kt_create_kernel(void *entry, unsigned long para);
void kt_delete_current();

//sleep.c
void kt_wakeup(struct ko_thread *who);
bool kt_sleep(unsigned long stat);

//sched.c
void kt_schedule();

//arch
void kt_arch_switch(struct ko_thread *prev, struct ko_thread *next);
void kt_arch_init_thread(struct ko_thread * thread, struct kt_thread_creating_context * ctx);

#endif

