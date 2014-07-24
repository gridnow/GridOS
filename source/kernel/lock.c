/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   自旋锁
 */

#include <kernel/ke_lock.h>
#include <kernel/ke_rwlock.h>

#include "spinlock.h"

#define __KE_SPIN_LOCK_UNLOCKED(lockname)	\
	(struct ke_spinlock) __RAW_SPIN_LOCK_INITIALIZER(lockname)
# define __spin_lock_init(lock)				\
	do { *(lock) = __KE_SPIN_LOCK_UNLOCKED(lock); } while (0)

DLLEXPORT void ke_spin_init(struct ke_spinlock * lock)
{
	__spin_lock_init(lock);
}
static int h2c(char *p,unsigned long hex)
{	
	int i;
	int j=((sizeof(void*))*8)/4;						//*8 means bits,/4 means a char represent 4 bits
	unsigned long mask,old=hex;

	j--;												//j-- means goto the end of the string
	for(i=j;i>=0;i--)
	{
		hex&=0xf;
		if(hex<=9)
			hex+='0';
		else
			hex='a'+(hex-0xa);
		*(p+i)=(char)hex;
		old>>=4;
		hex=old;
	}
	j++;												//goto the hex string end
	*(p+j)='h';
	j++;												//point to the next usable position
	return j;
}
DLLEXPORT void ke_spin_lock(struct ke_spinlock * lock)
{
//	char str[32];
	//str[h2c(str, __builtin_return_address(0))] = 0;
//	serial_puts("lock:", 1000);
//	serial_puts(str, 1000);
//	serial_puts("\n", 1000);
	arch_spin_lock((arch_spinlock_t*)lock);
}

DLLEXPORT void ke_spin_unlock(struct ke_spinlock * lock)
{
	arch_spin_unlock((arch_spinlock_t*)lock);
}

DLLEXPORT bool ke_spin_lock_try(struct ke_spinlock * lock)
{
	arch_spin_trylock((arch_spinlock_t*)lock);
}

/**
 @brief 自旋锁解锁，并回复IRQ状态
 */
DLLEXPORT void ke_spin_unlock_irqrestore(struct ke_spinlock * lock, unsigned long flags)
{
	_raw_spin_unlock_irqrestore((arch_spinlock_t*)lock, flags);
}

/**
 @brief 自旋锁加锁，关闭IRQ，并返回关闭IRQ前的状态
 */
DLLEXPORT unsigned long ke_spin_lock_irqsave(struct ke_spinlock * lock)
{
	unsigned long flags;
	_raw_spin_lock_irqsave((arch_spinlock_t*)lock, flags);
	return flags;
}

DLLEXPORT void ke_spin_lock_irq(struct ke_spinlock * lock)
{
	local_irq_disable();
	ke_spin_lock(lock);
}

DLLEXPORT void ke_spin_unlock_irq(struct ke_spinlock * lock)
{
	ke_spin_unlock(lock);
	local_irq_enable();
}

DLLEXPORT void ke_spin_lock_may_be_long(struct ke_spinlock * lock)
{
	//TODO
}

/************************************************************************/
/* About rwlock                                                         */
/************************************************************************/

/**
	@brief 初始化读写锁
*/
void ke_rwlock_init(struct ke_rwlock * rwlock)
{
	struct __rwlock_t
	{
		arch_rwlock_t raw_lock;
	} * wrap = (struct __rwlock_t*)rwlock;
	
	*wrap = (struct __rwlock_t)	{.raw_lock = __ARCH_RW_LOCK_UNLOCKED};
}

/**
	@brief 读写锁
*/
void ke_rwlock_write_lock(struct ke_rwlock * rwlock)
{
	arch_write_lock((arch_rwlock_t*)rwlock);
}

void ke_rwlock_read_lock(struct ke_rwlock * rwlock)
{
	arch_read_lock((arch_rwlock_t*)rwlock);
} 

/**
	@brief 读写锁，解写锁
*/
void ke_rwlock_write_unlock(struct ke_rwlock * rwlock)
{
	arch_write_unlock((arch_rwlock_t*)rwlock);
}

void ke_rwlock_read_unlock(struct ke_rwlock * rwlock)
{
	arch_read_unlock((arch_rwlock_t*)rwlock);
}
