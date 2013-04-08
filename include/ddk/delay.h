/**
*  @defgroup delay
*  @ingroup DDK
*
*  定义了DDK的延迟接口
*  @{
*/

#ifndef _DDK_DELAY_H_
#define _DDK_DELAY_H_


/**
	@brief Sleep milliseconds 

	休眠milliseconds并且不退出休眠直到时间流逝完
*/
void hal_msleep(unsigned int msecs);

/**
	@brief Sleep in us unit

	由于US的单位比较小，线程不一定进入休眠状态，可能进入轮询状态
*/
void hal_usleep(unsigned int us);

/**
	@brief Sleep in 1/1000 second unit
*/
#define msleep hal_msleep
#define udelay(us) hal_usleep(us)
#define MAX_UDELAY_MS	5
#define mdelay(n) ( \
	(__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? udelay((n)*1000) : \
	({unsigned long __ms=(n); while (__ms--) udelay(1000);}))

#endif
/** @} */
