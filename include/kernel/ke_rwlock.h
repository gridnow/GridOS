/**
   @defgroup lock
   @ingroup kernel_api
	
   �ں˱�̽ӿ� ��д��

   @{
*/
#ifndef KE_RW_LOCK_H
#define KE_RW_LOCK_H

struct ke_rwlock
{
	/* ʹ�����ļ�����,��������������2048ʱ��64λ */
	s64	lock;
};

/**
	@brief ��ʼ����д��
*/
void ke_rwlock_init(struct ke_rwlock * rwlock);

/**
	@brief ��д������д��

	���д���ӳɹ��������Ķ���������������
*/
void ke_rwlock_write_lock(struct ke_rwlock * rwlock);

/**
	@brief ��д������д��
*/
void ke_rwlock_write_unlock(struct ke_rwlock * rwlock);


/**
	@brief ��д�����Ӷ���

	�����ȡ���ǿ��Բ�����
*/
void ke_rwlock_read_lock(struct ke_rwlock * rwlock);

/**
	@brief ��д�����ͷŶ���
*/
void ke_rwlock_read_unlock(struct ke_rwlock * rwlock);

#endif
/** @} */
