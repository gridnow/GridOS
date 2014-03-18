/**
   @defgroup thread
   @ingroup kernel_api
	
   �ں˱�̽ӿ� ��д��

   @{
*/
#ifndef KE_THREAD_H
#define KE_THREAD_H

#include <types.h>

typedef unsigned long ke_thread;
struct y_message;

/**
	@brief ��ȡ��ǰ����,���ں˱��ʹ��
*/
ke_thread ke_current();

/**
	@brief �����ں��߳�
*/
ke_thread ke_create_kernel(void *entry, void *para);

/**
	@brief �ṩ���ں˱�̽ӿ�ʹ��
*/
bool ke_msg_send(ke_thread to, struct y_message *what);

#endif
/** @} */

