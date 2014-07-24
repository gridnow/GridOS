/**
	@defgroup mutex
	@ingroup kernel_api
 
	�ں˱�̽ӿ� Mutex
 
 @{
*/

#ifndef KE_MUTEX_H
#define KE_MUTEX_H
#include <types.h>
#include <compiler.h>
#include "kernel.h"

struct ke_mutex
{
	unsigned long reserved[KE_SYNC_BASE_OBJECT_RESERVE];										/* Room for kernel sync ops */
	unsigned long count;
	void *owner;
};

/**
	@brief ��ʼ��Mutex

	�κ�������������Mutex��������ȳ�ʼ��������ᵼ�²���Ԥ֪�Ĵ���

	@param[in] mutex ׼����ʼ������ЧMutex����ָ��
	@param[in] initial_ownership ��ʾ�Ƿ��ڳ�ʼ���׶ξ�ռ�ø�Mutex
*/
DLLEXPORT void ke_mutex_init(struct ke_mutex *mutex, bool intial_ownership);

/**
	@brief ��ȡMutex

	����һ��Mutex�������Mutex�Ѿ��������̻߳�ȡ����ǰ�߳�������ֱ�������߳̽�����Mutex��
	�����Mutex�Ѿ�����ǰ�̻߳�ȡ����ô���ν��ɹ���ȡ������Mutex��ʹ�ü���������ȡ������Ҫ�ͷż��Ρ�
	�����Mutexһ��Ҳû����ȡ����ô���ν�������ȡ������Mutex��ʹ�ü�������˼�����Ϊ1��

	@param[in] mutex ��Ч��Mutex����ָ��

	@return
		true ��ʾ�ɹ���ȡ��false��ʾmutex�Ѿ�������
*/
DLLEXPORT bool ke_mutex_lock(struct ke_mutex *mutex);

/**
	@brief �ͷ�Mutex

	��Mutex��ǰ����ȡ�����߶�λ�ȡ��Mutex������ס״̬��
	���߳��뿪������ʱ��һ��Ҫ�ͷŴ�����ס״̬��Mutex�����������̲߳����ٴλ�ȡ��Mutex��ִ�л������ڵĳ���
	���һ��Mutex��һ���̶߳�λ�ȡ����Ҫ����ͷţ��ͷŵĴ����ͽ�������һ��������ȫ�ͷ�Mutex��

	@param[in] mutex ��Ч��Mutex����ָ��

	@note
		���ִ�б��������̲߳��ǵ�����ȡ��Mutex���̣߳���ô�������κ��ͷŶ����ķ������ͷ���Ч��

	@return
		����ǰ�̻߳�ȡ�Ĵ�����1 ��ʾ�����ͷ��꣬>1 ��ʾ��ʷ�ϲ�ֹ����ȡ��һ�Σ�Ҫ�����ͷ�Mutex������ñ�����
*/
DLLEXPORT unsigned long ke_mutex_unlock(struct ke_mutex *mutex);

#endif
/** @} */