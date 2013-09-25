/**
	@defgroup complete
	@ingroup kernel_api

	�ں˱�̽ӿ� �����

	@{
*/
#ifndef KE_COMPLETE_H
#define KE_COMPLETE_H

#include <kernel/ke_lock.h>
#include <list.h>

/**
	@brief ����壬���Ը�֪�ȴ���ĳ�������Ѿ�����ɣ������ѵȴ���

	���¼���ȣ����������������ҿ����ڶ�ջ�д�����
*/
struct ke_completion 
{
	unsigned int done;
	struct list_head task_list;
	struct ke_spinlock lock;
};

/**
	@brief ��ʼ��һ�������

	�����Ҫ�ȱ�������һ����Ƕ�ڽṹ���еģ������߾�̬������
*/
DLLEXPORT void ke_init_completion(struct ke_completion * x);

/**
	@brief �ȴ�����崦�ھ���״̬

	�������ɣ���ô��һֱ������
*/
DLLEXPORT void ke_wait_for_completion(struct ke_completion * x);

/**
	@brief �ȴ�����崦�ھ���״̬�����������Ԥ��ʱ�����ٵȴ�

	��������һ�ִ��г�ʱ���ܵĵȴ������������ڸ�ʱ�������������ã�����һֱ�ȴ���ֱ����ʱ

	@param[in] x ��������
	@param[in] timeout ��ʱʱ�䣬��λ��system tick

	@return �����;��ɷ��ػ�ʣ�¶���ʱ��Ҫ�ȴ������򷵻�0��ʾ��ʱ
*/		
DLLEXPORT unsigned long ke_wait_for_completion_timeout(struct ke_completion * x, unsigned long timeout);

/**
	@brief �����ĳ�����飬���ѵ�һ���ȴ���
*/
DLLEXPORT void ke_complete(struct ke_completion * x);


#endif

/** @} */
