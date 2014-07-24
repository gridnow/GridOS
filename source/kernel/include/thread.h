/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   �߳�����
*/

#ifndef KT_THREAD_H
#define KT_THREAD_H

#include <arch/thread.h>
#include <list.h>
#include <spinlock.h>
#include <msg.h>

struct ko_process;
struct ko_thread
{
	struct kt_arch_thread arch_thread;
	struct ko_process *process;
	spinlock_t ops_lock;
	
	/* If this is a drvier thread, it should have ti */
	void *desc_of_driver;
	
	/* File */
	void *current_dir;
	
	/* Desc of user */
	void *teb;

	/* Messages */
	struct ktm msg;
	
	/* Scheduler */
	struct list_head queue_list;
	unsigned long state;
	unsigned char priority_level;
	unsigned int wakeup_count;
};

/* Context for thread creating */
struct kt_thread_creating_context
{
	unsigned long stack_pos;
	void *stack0;
	unsigned long stack0_size;
	void *fate_entry;
	void *thread_entry;
	int priority, cpl;
	unsigned long para;
	unsigned long flags;
};

#define KT_CREATE_STACK_AS_PARA			(1 << 1)
#define KT_CREATE_RUN					(1 << 2)

#define KT_STATE_RUNNING				1
#define KT_STATE_WAITING_SYNC			2
#define KT_STATE_WAITING_MSG			3
#define KT_STATE_KILLING				4
#define KT_STATE_MASK					(0xffff)

#define KT_STATE_ADD_UNINTERRUPT		(1 << 16)												//�������ԣ������жϵ���ĳ������
#define KT_STATE_ADD_FORCE_BY_SYSTEM	(1 << 17)												//�������ԣ�ǿ�����ߵģ����滽�ѣ��û���Ļ��ѣ��ǲ��ܵ�
#define KT_STATE_ADD_DIEING				(1 << 18)												//�������ԣ�Ҫ����...
#define KT_STATE_ADD_USING_FPU			(1 << 19)												//�������ԣ��̱߳���ʹ����FPU.
#define KT_STATE_ADD_NO_SWITCHING		(1 << 20)												//�������ԣ��̲߳������л��߳�.

#define KT_THREAD_FIRST_STACK_SIZE		(0x200000)

#define KT_GET_KP(THREAD)				((THREAD)->process)
#define KT_STATUS_BASE(THREAD)			((THREAD)->state & KT_STATE_MASK)
#define KT_CURRENT_KILLING()			((kt_current()->state & KT_STATE_MASK) == KT_STATE_KILLING)
#define KT_THREAD_KILLING(T)			(((T)->state & KT_STATE_MASK) == KT_STATE_KILLING)
#define KP_CURRENT()					(KT_GET_KP(kt_current()))
static inline struct ko_thread *kt_current()
{
	return kt_arch_get_current();
}

//thread.c
bool kt_init();

/**
	@brief Raw interface for create thread in kernel
*/
struct ko_thread *kt_create(struct ko_process * where, struct kt_thread_creating_context *ctx);

/**
	@brief Create kernel thread
*/
struct ko_thread *kt_create_kernel(void *entry, unsigned long para);

/**
	@brief Create thread for driver subsystem
*/
struct ko_thread *kt_create_driver_thread(void *ring0_stack, int stack_size, void *entry, unsigned long para);

void kt_delete_current();

//sleep.c
void kt_wakeup(struct ko_thread *who);
bool kt_sleep(unsigned long stat);

/**
	@brief Make a driver thread into running queue
 
	@note
		The thread must really be a driver thread.
*/
void kt_wakeup_driver(struct ko_thread *who);

//sched.c

/**
	@brief �л��߳�
*/
void kt_schedule();
void kt_schedule_driver();
#define KT_SWITCH_GIVE_CHANCE 1
void kt_switch(int why);

//arch
void kt_arch_switch(struct ko_thread *prev, struct ko_thread *next);
void kt_arch_init_thread(struct ko_thread * thread, struct kt_thread_creating_context * ctx);

#endif

