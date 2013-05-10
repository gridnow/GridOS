/**
 *  @defgroup DDKDebuger
 *  @ingroup DDK
 *
 *  ����������������Խӿ�
 *  @{
 */


#ifndef _DDK_DEBUG_H_
#define _DDK_DEBUG_H_

#include <types.h>

/**
	@brief ����Ļ���ߴ��ڵ��豸����ں˵��ַ������л�

	@return �ɹ�������ַ��ֽ���
*/
extern int printk(const char *fmt, ...);

/**
	@brief ���������Ϣ
*/

#define hal_printf printk
#define hal_printf_warning printk
#define hal_printf_error printk

/* prefix */
#define HAL_DEBUG		"HAL���ԣ�"
#define HAL_INFO		"HAL��ʾ��"
#define HAL_WARNING		"HAL���棺"
#define HAL_ERR		"HAL����"
#define KERN_INFO	HAL_INFO
#define KERN_WARNING HAL_WARNING
/* debug.c */
extern void hal_do_panic();

/**
	@brief hal �ڲ����� 
*/
#define hal_panic(buf) \
	do {	\
		printk("���ļ�%s�еĺ���%s��%d�г�����������:%s.\n", __FILE__, __FUNCTION__, __LINE__, buf);	\
		hal_do_panic();	\
	} while(0)	
#define TODO(__what__)	\
	do {	\
		printk("�ļ�%s�еĺ���%s��%d����Ҫ���� %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	
#define TRACE_UNIMPLEMENTED(__info__) \
	do {	\
		printk("�ļ�%s�е�%s�������к�%d��û��ʵ�֣�%s��\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_ERROR(__info__) \
	do {	\
	printk("�ļ�%s�е�%s�������к�%d����������%s��\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_POINT() \
	do {	\
		printk("�ļ�%s�е�%s�������к�%d�����С�\n", __FILE__,__FUNCTION__,__LINE__);	\
	} while(0)

/**
	@brief BUG ��ʾһ����Ӧ��ȥ������.
*/
#define BUG()	\
	do {	\
		printk("�ļ�%s�еĺ���%s��%d�в�Ӧ��ִ��,����BUG.\n", __FILE__, __FUNCTION__, __LINE__);	\
		hal_do_panic();	\
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
	@brief BUILD_BUG_ON ̽�����ʱ��һЩ�����
*/
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

#endif

/** @} */

