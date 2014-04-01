
#ifndef FSNOTIFY_H
#define FSNOTIFY_H

#include <vfs.h>
#include <ystd.h>
#include <kernel/ke_srv.h>
#include <kernel/ke_thread.h>



/*
	�ļ�ͨ���¼���ݽṹ,��Ƕ��fss_file�ṹ�ڼ�������
*/
struct file_notify
{
	/* ��������fs_file�ļ����� */
	struct list_head node;
	/* �����߳� */
	ke_thread  listener;
	/* �����¼����� */
	y_file_event_type_t even_type_mask;
	
	/* ע��ص���������� */
	void *call_func;
	void *call_para;
};

/*
	�ļ�ͨ����Ϣ��ʽ
*/
struct y_file_notify_msg
{
	/* ��Ϣ���� */
	y_file_event_type_t even_mask;
	/* ע��ص���������� */
	void *call_func;
	void *call_para;
};

/**
	@ brief �ļ�ͨ�溯��,���ļ������ı����
	@return 
		void 
*/

void fnotify_msg_send(struct fss_file *file, y_file_event_type_t even_mask);


/**
	@brief �ṩ�û�ע���ļ�ͨ���¼�
	@return
		�ɹ� = 0, < 0 ���ش�����
*/
int  fnotify_event_register(struct fss_file *file, y_file_event_type_t cmd, void *func, void *para);

/**
	@brief ȡ�������ļ��¼�����
	@return
		�ɹ� = 0, < 0 ���ش�����
*/
int fnotify_event_unregister(struct fss_file *file, y_file_event_type_t cmd);

#endif


