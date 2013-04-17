/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*/

/**
    @defgroup lock
    @ingroup kernel_api

    �ں˱�̽ӿ� ��

    @{
*/
#ifndef KE_LOCK_H
#define KE_LOCK_H

#include <types.h>
typedef struct ke_spinlock
{
	unsigned long dummy;
} spinlock_t;

/**
	@brief ������һ������

	���lock�Ѿ�������߳���ס����ô��������������ֱ�������߳̽���

	@param[in] lock The lock to operate on
*/
void ke_spin_lock(struct ke_spinlock * lock);

/**
	@brief ������һ������

	�������ס��������������������˳�.

	@param[in] lock The lock to operate on
	
	@return true on success or false on failed
*/
bool ke_spin_lock_try(struct ke_spinlock * lock);

/**
	@brief ������

	�⿪���̶߳�lock����������������������߳��ڵȴ���������Щ�̣߳��Ӷ��ٴξ�����
*/
void ke_spin_unlock(struct ke_spinlock * lock);

/**
	@brief ��ʼ��������

	���������������г�ʼ����Ϊ������׼��
*/
void ke_spin_init(struct ke_spinlock * lock);

/**
	@brief ���������ܳ�ʱ�����

	���ӿڸ�֪����ϵͳ�ںˣ�����������һʱ���ᱻ�⿪����������ĳЩ������ʱ��������
	�����ں��ڵ��ȵ�ʱ����Խ������ȴ��ŵ����ȼ���ʱ���ͣ�����ת���ɡ��еȡ������ǡ�æ�ȡ����Խ�ʡCPUʱ�䡣

*/
void ke_spin_lock_may_be_long(struct ke_spinlock * lock);

/**
	@brief ���������������ظ�IRQ״̬
*/
void ke_spin_unlock_irqrestore(struct ke_spinlock * lock, unsigned long flags);

/**
	@brief �������������ر�IRQ�������عر�IRQǰ��״̬
*/
unsigned long ke_spin_lock_irqsave(struct ke_spinlock * lock);

/**
	@brief �ر��ж�����
*/
void ke_spin_lock_irq(struct ke_spinlock * lock);

/**
	@brief ���� + ���ж�
*/
void ke_spin_unlock_irq(struct ke_spinlock * lock);

/* Old interface */
#define spin_lock_init ke_spin_init
#endif

/** @} */
