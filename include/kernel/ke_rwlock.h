/**
   @defgroup lock
   @ingroup kernel_api
	
   内核编程接口 读写锁

   @{
*/
#ifndef KE_RW_LOCK_H
#define KE_RW_LOCK_H

struct ke_rwlock
{
	/* 使用最大的计数器,处理器数量大于2048时用64位 */
	s64	lock;
};

/**
	@brief 初始化读写锁
*/
void ke_rwlock_init(struct ke_rwlock * rwlock);

/**
	@brief 读写锁，加写锁

	如果写锁加成功，后续的读或者锁都将阻塞
*/
void ke_rwlock_write_lock(struct ke_rwlock * rwlock);

/**
	@brief 读写锁，解写锁
*/
void ke_rwlock_write_unlock(struct ke_rwlock * rwlock);


/**
	@brief 读写锁，加读锁

	多个读取者是可以并发的
*/
void ke_rwlock_read_lock(struct ke_rwlock * rwlock);

/**
	@brief 读写锁，释放读锁
*/
void ke_rwlock_read_unlock(struct ke_rwlock * rwlock);

#endif
/** @} */
