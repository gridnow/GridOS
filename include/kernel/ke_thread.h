
#ifndef KE_THREAD_H
#define KE_THREAD_H

typedef unsigned long ke_thread;

/*
	@brief ��ȡ��ǰ����,���ں˱��ʹ��
*/
ke_thread ke_current();

/*
	@brief �ṩ���ں˱�̽ӿ�ʹ��
*/
bool ke_msg_send(ke_thread to, struct y_message *what);

#endif


