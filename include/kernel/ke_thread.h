
#ifndef KE_THREAD_H
#define KE_THREAD_H

typedef unsigned long ke_thread;

/*
	@brief ��ȡ��ǰ����,���ں˱��ʹ��
*/
struct ko_thread *ke_current();

/*
	@brief �ṩ���ں˱�̽ӿ�ʹ��
*/
bool ke_send(struct ko_thread *to, struct y_message *what);

#endif


