/**
	@defgroup complete
	@ingroup kernel_api

	内核编程接口 完成体

	@{
*/
#ifndef KE_COMPLETE_H
#define KE_COMPLETE_H

#include <kernel/ke_lock.h>
#include <list.h>

/**
	@brief 完成体，用以告知等待着某个事情已经被完成，并唤醒等待者

	和事件相比，它更轻量级，而且可以在堆栈中创立。
*/
struct ke_completion 
{
	unsigned int done;
	struct list_head task_list;
	struct ke_spinlock lock;
};

/**
	@brief 初始化一个完成体

	完成体要先被创立（一般是嵌在结构体中的），或者静态变量。
*/
DLLEXPORT void ke_init_completion(struct ke_completion * x);

/**
	@brief 等待完成体处于就绪状态

	如果不完成，那么线一直程阻塞
*/
DLLEXPORT void ke_wait_for_completion(struct ke_completion * x);

/**
	@brief 等待完成体处于就绪状态，如果超过了预定时间则不再等待

	本函数是一种带有超时功能的等待，如果完成体在该时间内能完成则最好，否则一直等待，直到超时

	@param[in] x 完成体对象
	@param[in] timeout 超时时间，单位是system tick

	@return 如果中途完成返回还剩下多少时间要等待；否则返回0表示超时
*/		
DLLEXPORT unsigned long ke_wait_for_completion_timeout(struct ke_completion * x, unsigned long timeout);

/**
	@brief 完成了某个事情，唤醒第一个等待着
*/
DLLEXPORT void ke_complete(struct ke_completion * x);


#endif

/** @} */
