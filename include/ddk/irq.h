/**
*  @defgroup IRQ
*  @ingroup DDK
*
*  �������ж���ؽӿڣ�����ʹ���жϣ�ע�ᡢ��ע���жϴ������Ƚӿڡ�
*  @{
*/

#ifndef _DDK_IRQ_H_
#define _DDK_IRQ_H_

/**
	@brief �ر�cpu���ж�ʹ��λ

	�رյ�ǰ���������жϣ������عر�ǰ���ж�״̬

	@return
		�ر�ǰ���ж�״̬��һ�����ڻָ�
*/
unsigned long hal_local_irq_save();

/**
	@brief �ָ�cpu���ж�ʹ��λ
*/
void hal_local_irq_restore(unsigned long flags);

/**
	@brief �����жϴ������Ľ��
*/
enum __irqreturn__ {
	IRQ_NONE		= (0 << 0),									/**< interrupt was not from this device */
	IRQ_HANDLED		= (1 << 0),									/**< interrupt was handled by this device */
	IRQ_WAKE_THREAD		= (1 << 1),								/**< handler requests to wake the handler thread */
};
typedef enum __irqreturn__ irqreturn_t;
#define IRQ_RETVAL(x)	((x) != IRQ_NONE)
typedef irqreturn_t (*irq_handler_t)(int, void *);

/*  �����ж�ʱ��flag */
#define IRQF_SHARED		0x00000080
#endif

/** @} */
