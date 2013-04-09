/**
*  @defgroup Log
*  @ingroup DDK
*
*  ������������־��¼���ܣ����ֽӿڿ��Ǽ�����������
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
#define __WARN() printk("���棺%s %d��������.\n", __FILE__, __LINE__)


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

/* ��¼�ض��豸���������������־,TODO to support this */
#define dev_info(dev, format, arg...) printk("�豸��Ϣ��"format, ##arg)
#define dev_dbg dev_info
#define dev_warn dev_info
#define dev_printk(level, dev, format, arg...) printk("�豸��Ϣ��"format, ##arg)
#define dev_err dev_info
#define dev_trace(format, arg...) printk("�豸���٣�"format, ##arg)
#define dev_WARN(dev, format, arg...) printk("�豸����(%s.%d):"format"\n", __FUNCTION__, __LINE__, ##arg)

/* ��Ҫʵ�ֵ�ȴ��û���ü�ʵ�ֵĺ����д�ӡ������Ϣ��Ҳ�������ڵ��Ը��ٳ������� */
#define TODO(__what__)	\
	do {	\
	printk("�ļ�%s�еĺ���%s��%d����Ҫ���� %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	
#define UNUSED(__what__)	\
	do {	\
	printk("�ļ�%s�еĺ���%s��%d������ %s.\n", __FILE__, __FUNCTION__, __LINE__, __what__);	\
	} while(0)	

#endif

/** @} */