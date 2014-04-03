/**
	@defgroup mutex
	@ingroup kernel_api
 
	内核编程接口 Mutex
 
 @{
*/
#ifndef KE_H
#define KE_H

typedef unsigned int page_prot_t;
#define KM_PROT_READ			(1 << 0)
#define KM_PROT_WRITE			(1 << 1)
#define KM_MAP_DEVICE			(1 << 16)
#define KM_MAP_ANON				(1 << 17)
#define KM_MAP_ARCH_SPECIAL		(1 << 31/*Arch special map */)

#define Y_SYNC_MAX_OBJS_COUNT 64
#define KE_SYNC_WAIT_INFINITE (-1UL)

/* Should met the size of kt_sync_base */
#define KE_SYNC_BASE_OBJECT_RESERVE 4
typedef enum
{
	KE_WAIT_OK = 0,
	KE_WAIT_TIMEDOUT,
	KE_WAIT_ABANDONED,
}kt_sync_wait_result;

/*******************************************************/
/* About thread */
/*******************************************************/
/**
	@brief 当前线程等待

	当前线程等待tiemout的过去，timeout 是时间滴答数，特别注意，在不同的系统上，滴答的频率不一定一样。
	并且只有等滴答完了后线程才会继续运行，除此之外无论发生什么情况线程都不会退出。
	该接口一般是驱动程序使用，不推荐在其他地方使用，因为它会导致线程无法立即杀掉。

	@param[in] timeout 要等待的滴答数

	@return
		是0。
*/
signed long ke_sleep_timeout(signed long timeout);

#endif
/** @} */