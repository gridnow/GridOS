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
	@brief Sleep in 1/1000 second unit
*/
#define msleep ke_msleep
#define udelay(us) ke_usleep(us)
#define MAX_UDELAY_MS	5
#define mdelay(n) ( \
	(__builtin_constant_p(n) && (n)<=MAX_UDELAY_MS) ? udelay((n)*1000) : \
	({unsigned long __ms=(n); while (__ms--) udelay(1000);}))

#endif
/** @} */