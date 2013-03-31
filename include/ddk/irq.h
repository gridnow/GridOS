/**
*  @defgroup IRQ
*  @ingroup DDK
*
*  定义了中断相关接口，比如使能中断，注册、反注册中断处理函数等接口。
*  @{
*/

#ifndef _DDK_IRQ_H_
#define _DDK_IRQ_H_

/**
	@brief 关闭cpu的中断使能位

	关闭当前处理器的中断，并返回关闭前的中断状态

	@return
		关闭前的中断状态，一般用于恢复
*/
unsigned long hal_local_irq_save();

/**
	@brief 恢复cpu的中断使能位
*/
void hal_local_irq_restore(unsigned long flags);

/**
	@brief 描述中断处理函数的结果
*/
enum __irqreturn__ {
	IRQ_NONE		= (0 << 0),									/**< interrupt was not from this device */
	IRQ_HANDLED		= (1 << 0),									/**< interrupt was handled by this device */
	IRQ_WAKE_THREAD		= (1 << 1),								/**< handler requests to wake the handler thread */
};
typedef enum __irqreturn__ irqreturn_t;
#define IRQ_RETVAL(x)	((x) != IRQ_NONE)
typedef irqreturn_t (*irq_handler_t)(int, void *);

/*  请求中断时的flag */
#define IRQF_SHARED		0x00000080
#endif

/** @} */
