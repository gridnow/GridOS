/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL 调试相关
*/

#ifndef HAL_DEBUG_H
#define HAL_DEBUG_H

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
#define KERN_INFO	HAL_INFO
#define KERN_WARNING HAL_WARNING
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
		printk("文件%s中的函数%s第%d行需要完善 %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	
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

/**
	@brief BUG 表示一个不应该去的流程.
*/
#define BUG()	\
	do {	\
		printk("文件%s中的函数%s第%d行不应该执行.\n", __FILE__, __FUNCTION__, __LINE__);	\
		ke_panic("BUG");	\
	} while(0)	
#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while(0)

#define __WARN_printf(arg...)	do { printk(arg); } while (0)

#define WARN_ON(condition) ({						\
	int __ret_warn_on = !!(condition);				\
	unlikely(__ret_warn_on);					\
})

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

#endif

