/**
*  @defgroup Log
*  @ingroup DDK
*
*  定义了驱动日志记录功能，部分接口考虑兼容现有驱动
*  @{
*/

#ifndef _DDK_LOG_H_
#define _DDK_LOG_H_

#include <compiler.h>
/* Record log */
#define KERN_ERR
#define KERN_WARNING
#define KERN_INFO
#define KERN_DEBUG
extern void printk(const char * fmt, ...);

/* Runtime sanity check */
#define __WARN() printk("警告：%s %d行有问题.\n", __FILE__, __LINE__)


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

/* 记录特定设备对象的驱动运行日志,TODO to support this */
#define dev_info(dev, format, arg...) printk("设备信息："format, ##arg)
#define dev_dbg dev_info
#define dev_warn dev_info
#define dev_printk(level, dev, format, arg...) printk("设备信息："format, ##arg)
#define dev_err dev_info
#define dev_trace(format, arg...) printk("设备跟踪："format, ##arg)
#define dev_WARN(dev, format, arg...) printk("设备警告(%s.%d):"format"\n", __FUNCTION__, __LINE__, ##arg)

/* 在要实现的却还没来得及实现的函数中打印跟踪信息，也可以用于调试跟踪程序流程 */
#define TODO(__what__)	\
	do {	\
	printk("文件%s中的函数%s第%d行需要完善 %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	
#define UNUSED(__what__)	\
	do {	\
	printk("文件%s中的函数%s第%d行无需 %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	

#endif

/** @} */