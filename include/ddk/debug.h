/**
 *  @defgroup DDKDebuger
 *  @ingroup DDK
 *
 *  ������������־��¼���ܣ����ֽӿڿ��Ǽ�����������
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

#ifndef PRINT 
#define PRINT printk
#endif

/**
	@brief ���������Ϣ
*/

#define hal_printf PRINT
#define hal_printf_warning PRINT
#define hal_printf_error PRINT

/* prefix */
#define HAL_DEBUG		"HAL���ԣ�"
#define HAL_INFO		"HAL��ʾ��"
#define HAL_WARNING		"HAL���棺"
#define HAL_ERR		"HAL����"

#define KERN_ERR
#define KERN_WARNING
#define KERN_INFO
#define KERN_DEBUG

/* debug.c */
extern void hal_do_panic();

/**
	@brief hal �ڲ����� 
*/
#define hal_panic(buf) \
	do {	\
		PRINT("���ļ�%s�еĺ���%s��%d�г�����������:%s.\n", __FILE__, __FUNCTION__, __LINE__, buf);	\
		hal_do_panic();	\
	} while(0)	
#define TODO(__what__)	\
	do {	\
		PRINT("%s->%s line %d TODO %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)
#define TODO_ROLL_BACK() \
	do {	\
		TODO("������Ҫ����ǰ��Ĳ���");	\
	} while (0)
#define TRACE_UNIMPLEMENTED(__info__) \
	do {	\
		PRINT("�ļ�%s�е�%s�������к�%d��û��ʵ�֣�%s��\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_ERROR(__info__) \
	do {	\
	PRINT("�ļ�%s�е�%s�������к�%d����������%s��\n", __FILE__,__FUNCTION__,__LINE__,__info__);	\
	} while(0)
#define TRACE_POINT() \
	do {	\
		PRINT("�ļ�%s�е�%s�������к�%d�����С�\n", __FILE__,__FUNCTION__,__LINE__);	\
	} while(0)
#define UNUSED(__what__)	\
	do {	\
	PRINT("�ļ�%s�еĺ���%s��%d������ %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	

/**
	@brief BUG ��ʾһ����Ӧ��ȥ������.
*/
#define BUG()	\
	do {	\
		PRINT("�ļ�%s�еĺ���%s��%d�в�Ӧ��ִ��,����BUG.\n", __FILE__, __FUNCTION__, __LINE__);	\
		hal_do_panic();	\
	} while(0)	
#define BUG_ON(condition) do { if (unlikely(condition)) BUG(); } while(0)

#define __WARN_printf(arg...)	do { PRINT(arg); } while (0)

/* Ŀǰ��Щwarn��������������������� */
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
	@brief BUILD_BUG_ON ̽�����ʱ��һЩ�����
*/
#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))

/* Runtime sanity check */
#define __WARN() PRINT("���棺%s %d��������.\n", __FILE__, __LINE__)

/* ��¼�ض��豸���������������־,TODO to support this */
#define dev_info(dev, format, arg...) PRINT("dev_inf��"format, ##arg)
#define dev_dbg dev_info
#define dev_warn dev_info
#define dev_printk(level, dev, format, arg...) PRINT("�豸��Ϣ��"format, ##arg)
#define dev_err dev_info
#define dev_trace(format, arg...) PRINT("�豸���٣�"format, ##arg)
#define dev_WARN(dev, format, arg...) PRINT("�豸����(%s.%d):"format"\n", __FUNCTION__, __LINE__, ##arg)

#endif

/** @} */