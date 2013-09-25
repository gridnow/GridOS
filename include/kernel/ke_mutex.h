/**
	@defgroup mutex
	@ingroup kernel_api
 
	内核编程接口 Mutex
 
 @{
*/

#ifndef KE_MUTEX_H
#define KE_MUTEX_H
#include <types.h>
#include <compiler.h>
#include "kernel.h"

struct ke_mutex
{
	unsigned long reserved[KE_SYNC_BASE_OBJECT_RESERVE];										/* Room for kernel sync ops */
	unsigned long count;
	void *owner;
};

/**
	@brief 初始化Mutex

	任何能正常工作的Mutex对象必须先初始化，否则会导致不可预知的错误。

	@param[in] mutex 准备初始化的有效Mutex对象指针
	@param[in] initial_ownership 表示是否在初始化阶段就占用该Mutex
*/
DLLEXPORT void ke_mutex_init(struct ke_mutex *mutex, bool intial_ownership);

/**
	@brief 获取Mutex

	竞争一个Mutex，如果该Mutex已经被其他线程获取，当前线程阻塞，直到其他线程解锁该Mutex。
	如果该Mutex已经被当前线程获取，那么本次将成功获取并增加Mutex的使用计数器，获取几次需要释放几次。
	如果该Mutex一次也没被获取，那么本次将唱功获取并增加Mutex的使用计数，因此计数器为1。

	@param[in] mutex 有效的Mutex对象指针

	@return
		true 表示成功获取，false表示mutex已经不可用
*/
DLLEXPORT bool ke_mutex_lock(struct ke_mutex *mutex);

/**
	@brief 释放Mutex

	当Mutex以前被获取，或者多次获取后，Mutex处于锁住状态。
	当线程离开互斥区时，一般要释放处于锁住状态的Mutex，这样其他线程才能再次获取到Mutex并执行互斥区内的程序。
	如果一个Mutex被一个线程多次获取，则要多次释放，释放的次数和解锁次数一样才能完全释放Mutex。

	@param[in] mutex 有效的Mutex对象指针

	@note
		如果执行本函数的线程不是当初获取该Mutex的线程，那么不会有任何释放动作的发生，释放无效！

	@return
		被当前线程获取的次数，1 表示彻底释放完，>1 表示历史上不止被获取了一次，要彻底释放Mutex还需调用本函数
*/
DLLEXPORT unsigned long ke_mutex_unlock(struct ke_mutex *mutex);

#endif
/** @} */