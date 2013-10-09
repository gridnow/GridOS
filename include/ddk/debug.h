/**
 *  @defgroup DDKDebuger
 *  @ingroup DDK
 *
 *  定义了驱动日志记录功能，部分接口考虑兼容现有驱动
 *  @{
 */
#ifndef _DDK_DEBUG_H_
#define _DDK_DEBUG_H_

#include <types.h>

/**
	@brief 向屏幕或者串口等设备输出内核的字符串序列化

	@return 成功输出的字符字节数
*/
extern int printk(const char *fmt, ...);

/**
	@brief 输出调试信息
*/

#define hal_printf printk
#define hal_printf_warning printk
#define hal_printf_error printk

/* prefix */
#define HAL_DEBUG		"HAL调试："
#define HAL_INFO		"HAL提示："
#define HAL_WARNING		"HAL警告："
#define HAL_ERR		"HAL错误："

#define KERN_ERR
#define KERN_WARNING
#define KERN_INFO
#define KERN_DEBUG

/* debug.c */
extern void hal_do_panic();

/**
	@brief hal 内部错误 
*/
#define hal_panic(buf) \
	do {	\
		printk("在文件%s中的函数%s第%d行出现致命问题:%s.\n", __FILE__, __FUNCTION__, __LINE__, buf);	\
		hal_do_panic();	\
	} while(0)	
#define TODO(__what__)	\
	do {	\
		printk("%s->%s line %d TODO %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)
#define TODO_ROLL_BACK() \
	do {	\
		TODO("出错需要撤销前面的操作");	\
	} while (0)
#define TRACE_UNIMPLEMENTED(__info__) \
	do {	\
		printk("文件%s中的%s函数（行号%d）没有实现：%s。\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_ERROR(__info__) \
	do {	\
	printk("文件%s中的%s函数（行号%d）发生错误：%s。\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_POINT() \
	do {	\
		printk("文件%s中的%s函数（行号%d）运行。\n", __FILE__,__FUNCTION__,__LINE__);	\
	} while(0)
#define UNUSED(__what__)	\
	do {	\
	printk("文件%s中的函数%s第%d行无需 %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	

/**
	@brief BUG 表示一个不应该去的流程.
*/
#define BUG()	\
	do {	\
		printk("文件%s中的函数%s第%d行不应该执行,程序BUG.\n", __FILE__, __FUNCTION__, __LINE__);	\
		hal_do_panic();	\
	} while(0)	
#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while(0)

#define __WARN_printf(arg...)	do { printk(arg); } while (0)

/* 目前有些warn的条件不好做，因此留空 */
#define WARN_ON(x) (0)

/* The real version of WARN_ON */
#ifndef WARN_ON
#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
	__WARN();						\
	unlikely(__ret_warn_on);					\
})
#endif

#ifndef WARN_ON_ONCE
#define WARN_ON_ONCE(condition)	({				\
	static bool __section(.data.unlikely) __warned;		\
	int __ret_warn_once = !!(condition);			\
	\
	if (unlikely(__ret_warn_once))				\
	if (WARN_ON(!__warned)) 			\
	__warned = true;			\
	unlikely(__ret_warn_once);				\
})
#endif

#define WARN(condition, format...) ({					\
	int __ret_warn_on = !!(condition);				\
	if (unlikely(__ret_warn_on))					\
		__WARN_printf(format);					\
	unlikely(__ret_warn_on);					\
})

/**
	@brief BUILD_BUG_ON 探测编译时的一些宏错误
*/
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

/* Runtime sanity check */
#define __WARN() printk("警告：%s %d行有问题.\n", __FILE__, __LINE__)

/* 记录特定设备对象的驱动运行日志,TODO to support this */
#define dev_info(dev, format, arg...) printk("dev_inf："format, ##arg)
#define dev_dbg dev_info
#define dev_warn dev_info
#define dev_printk(level, dev, format, arg...) printk("设备信息："format, ##arg)
#define dev_err dev_info
#define dev_trace(format, arg...) printk("设备跟踪："format, ##arg)
#define dev_WARN(dev, format, arg...) printk("设备警告(%s.%d):"format"\n", __FUNCTION__, __LINE__, ##arg)

#endif

/** @} */