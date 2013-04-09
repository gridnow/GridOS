/**
*  @defgroup delay
*  @ingroup DDK
*
*  ������DDK���ӳٽӿ�
*  @{
*/

#ifndef _DDK_DELAY_H_
#define _DDK_DELAY_H_


/**
	@brief Sleep milliseconds 

	����milliseconds���Ҳ��˳�����ֱ��ʱ��������
*/
void hal_msleep(unsigned int msecs);

/**
	@brief Sleep in us unit

	����US�ĵ�λ�Ƚ�С���̲߳�һ����������״̬�����ܽ�����ѯ״̬
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
