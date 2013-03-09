/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
    @defgroup lock
    @ingroup kernel_api

    内核编程接口 锁

    @{
*/
#ifndef KE_LOCK_H
#define KE_LOCK_H

#include <types.h>
typedef struct ke_spinlock
{
	unsigned long dummy;
} spinlock_t;

/**
	@brief 自旋锁一个对象

	如果lock已经被别的线程锁住，那么本函数将阻塞，直到其他线程解锁

	@param[in] lock The lock to operate on
*/
void ke_spin_lock(struct ke_spinlock * lock);

/**
	@brief 尝试锁一个对象

	如果能锁住则锁，如果被别人所则退出.

	@param[in] lock The lock to operate on
	
	@return true on success or false on failed
*/
bool ke_spin_lock_try(struct ke_spinlock * lock);

/**
	@brief 解自旋

	解开本线程对lock的锁，如果该锁上有其他线程在等待，则唤醒这些线程，从而再次竞争锁
*/
void ke_spin_unlock(struct ke_spinlock * lock);

/**
	@brief 初始化自旋锁

	对自旋锁变量进行初始化，为加锁做准备
*/
void ke_spin_init(struct ke_spinlock * lock);

/**
	@brief 自旋锁可能长时间加锁

	本接口告知操作系统内核，自旋锁可能一时不会被解开，可能由于某些操作长时间阻塞，
	这样内核在调度的时候可以将其他等待着的优先级暂时降低，甚至转换成“闲等”而不是“忙等”，以节省CPU时间。

*/
void ke_spin_lock_may_be_long(struct ke_spinlock * lock);

/**
	@brief 自旋锁解锁，并回复IRQ状态
*/
void ke_spin_unlock_irqrestore(struct ke_spinlock * lock, unsigned long flags);

/**
	@brief 自旋锁加锁，关闭IRQ，并返回关闭IRQ前的状态
*/
unsigned long ke_spin_lock_irqsave(struct ke_spinlock * lock);

/**
	@brief 关闭中断再锁
*/
void ke_spin_lock_irq(struct ke_spinlock * lock);

/**
	@brief 解锁 + 开中断
*/
void ke_spin_unlock_irq(struct ke_spinlock * lock);

#endif

/** @} */
