/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   同步方法抽象层
*/
#ifndef KT_SYNC_H
#define KT_SYNC_H

#include <list.h>
#include <spinlock.h>
#include <kernel/kernel.h>

#include <thread.h>

/**
	线程阻塞的点
*/
struct thread_wait
{
	struct ko_thread * who;
	struct list_head task_list;
};
#if 1 /* 等待条件的完成，暂时没有用到漂亮的宏 */
#define KT_DELETE_WAIT(WAIT_NODE) \
	list_del(&(WAIT_NODE)->task_list)
#define KT_PREPARE_WAIT(WAIT_QUEUE, WAIT_NODE) do { \
	(WAIT_NODE)->who = kt_current(); \
	list_add_tail(&(WAIT_NODE)->task_list, &(WAIT_QUEUE)); \
	}while (0)

#define KT_WAIT(__condition__)  \
({									\
	unsigned long __ret__ = KE_WAIT_OK; \
	do { \
		if ((__condition__)) \
			break;				\
		/* Sleep */ \
		kt_sleep(0); \
		\
		/* Dieing thread */ \
		if (KT_CURRENT_KILLING()) \
		{	\
			__ret__ = KE_WAIT_ABANDONED; \
			break; \
		}	\
		\
		/* TODO: APC Event */	\
	} while(1); \
	__ret__; \
})
#endif

#define KE_SYNC_STATIC_WAIT_NODE_COUNT 4
#define KE_SYNC_OBJ_LOCK(x) spin_lock(&((struct kt_sync_base*)(x))->lock)
#define KE_SYNC_OBJ_UNLOCK(x) spin_unlock(&((struct kt_sync_base*)(x))->lock)
#define TO_SYNC(x) ((struct kt_sync_base*)x)
#define TO_SYNCS(x) ((struct kt_sync_base**)x)

/* SYNC 方法 */
typedef enum
{
	KE_SYNC_SIGNALED_OK,
	KE_SYNC_SIGNALED_CANCEL,
	KE_SYNC_SIGNALED_NO,
	KE_SYNC_SATISFY_OK,
}kt_sync_status;

struct kt_sync_base;
struct kt_sync_ops
{
	kt_sync_status (*satisfied)(struct kt_sync_base * which, struct ko_thread * who);
	kt_sync_status (*signaled) (struct kt_sync_base * which, struct ko_thread * who);
};

/* Base of sync object, any change of size should met KE_SYNC_BASE_OBJECT_RESERVE */
struct kt_sync_base
{
	struct list_head wait_queue;
	spinlock_t lock;
	struct kt_sync_ops * ops;
};

//SYNC.C
signed long kt_timeout(struct ko_thread * who, signed long timeout);
void kt_sync_init(struct kt_sync_base * sync, struct kt_sync_ops * ops);
int kt_sync_wakeup(struct kt_sync_base * sync, int count);
kt_sync_wait_result kt_wait_object(struct ko_thread * who, struct kt_sync_base *p, unsigned int timeout);
kt_sync_wait_result kt_wait_objects(struct ko_thread * who, int count, struct kt_sync_base ** objects, bool wait_all, unsigned int timeout, int *id);

#endif
