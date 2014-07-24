/**
	@defgroup Event
	@ingroup kernel_api
 
	�ں˱�̽ӿ� Event
 
 @{
*/

#ifndef KE_EVENT_H
#define KE_EVENT_H
#include <types.h>
#include <compiler.h>
#include "kernel.h"

struct ke_event
{
	unsigned long reserved[KE_SYNC_BASE_OBJECT_RESERVE];										/* Room for kernel sync ops */
	char			initial;
	char			manual_reset;
	int				status;
};

/**
	@brief ��ʼ��һ���¼�

	�¼��ڱ�ʹ��ǰҪ�ȳ�ʼ����

	@param[in] event �¼�����Чָ��
	@param[in] manual_reset �¼��Ƿ����ֶ���λ��
	@param[in] initial_status �¼��ĳ�ʼ״̬�Ƿ��Ǵ���״̬������ǣ���ô��һ���ȴ��Ž�������
*/
void ke_event_init(struct ke_event *event, bool manual_reset, bool initial_status);

/**
	@brief ����һ���¼�

	������߳����ڵȴ��¼��ķ�������ô�����¼������������̱߳����ѣ��Ӷ�����Ϣ֪ͨ�����á�
	������¼����ֶ���λ�ģ���ô���еȴ����߳̽��������ѡ�
	������¼����Զ���λ�ģ���ôֻ���ѵȴ���������ǰ����̡߳�

	@param[in] event �¼�����Чָ��
 
	@return
		���ػ��ѵ��߳�����
*/
int ke_event_set(struct ke_event *event);

/**
	@brief ��λһ���¼�

	�¼�������ֶ���λ�ģ���ô�������¼����Ҫ���临λ���ܼ��������µ��¼���
	��������һֱ���ڴ���״̬�����ղ����µ��¼��ĵ�����
	
	@param[in] event �¼�����Чָ��
*/
void ke_event_reset(struct ke_event *event);

/**
	@brief �ȴ�һ���¼�����

	�����ȴ�һ���¼�������

	@param[in] event �¼�����Чָ��
	@param[in] timeout ��ʱ��λΪ1/1000�룬���ΪKE_SYNC_WAIT_INFINITE�����ʾ������ʱ

	@return
		�ȴ����
*/
kt_sync_wait_result ke_event_wait(struct ke_event * event, unsigned int timeout);

/**
	@brief �ȴ�һЩ�е��¼�����

	�����ȴ�һ���¼�������

	@param[in] count �¼�����
	@param[in] events �¼�����Чָ������
	@param[in] wait_all �Ƿ�Ҫ�ȴ������¼�������������˳�
	@param[in] timeout ��ʱ��λΪ1/1000�룬���ΪKE_SYNC_WAIT_INFINITE�����ʾ������ʱ
	@param[in,out] id ��һ�����������¼���id

	@return
		��һ������Event���¼�����ID�͵ȴ����
*/
kt_sync_wait_result ke_events_wait(int count, struct ke_event * events[], bool wait_all, unsigned int timeout, int *id);

#endif
/** @} */