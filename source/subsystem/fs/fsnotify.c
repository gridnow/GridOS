
#include <fsnotify.h>
#include <kernel/ke_lock.h>
#include <kernel/ke_thread.h>
#include <node.h>
#include <message.h>
#include <kernel/ke_memory.h>
#include <errno.h>

#define DBGMSG(form, ...) printk(form , ##__VA_ARGS__)

/* ����struct file_notify */
#define alloc_notify_file(fnotify) do{\
	fnotify = (struct file_notify *)km_valloc(sizeof(struct file_notify));\
	if (fnotify)\
	{\
		memset(fnotify, 0, sizeof(struct file_notify));\
		INIT_LIST_HEAD(&fnotify->node);\
	}\
	}while(0)

/* �ͷ� file notify */
#define free_notify_file(fnotify) do{\
	km_vfree(fnotify);\
	fnotify = NULL;\
	}while(0)


/*
	@brief ���ļ������߷����¼���Ϣ
	@return 
		��
*/
void fnotify_msg_send(struct fss_file *file, y_file_event_type_t even)
{
	struct file_notify *ntf_file = NULL;

	ke_spin_lock(&file->notify_lock);
	
	/* �����м������̷�����Ϣ */
	list_for_each_entry(ntf_file, &file->notify_list, node)
	{
		/* �Ƿ�Ϊ��������Ȥ�¼� ? */
		if (ntf_file->even_type_mask & even)
		{
			/* ���췢����Ϣ */
			MSG_MAKE(1, MSG_FLAGS_ADDRESS, (ntf_file->call_func))
			*pdata = (MSG_DATA_TYPE)(ntf_file->call_para);
			
			/* send msg */
			ke_msg_send(ntf_file->listener, pmsg);
		}
			
	}
	ke_spin_unlock(&file->notify_lock);
	
	return;
}

/*
	@brief ���ϲ����ʵ��ע������ļ�
	@return 
		= 0 �ɹ�  < 0 ���ش�����
*/
int  fnotify_event_register(struct fss_file *file, y_file_event_type_t cmd, void *func, void *para)
{
	struct file_notify *notify;
	int err;
	
	/* cmd�Ƿ�Ϸ�? */
	if (cmd & (~(Y_FILE_EVENT_READ | Y_FILE_EVENT_WRITE)))
	{
		err = -E2BIG;
		goto err;
	}

	/* ����,����notify.TODO ��Ҫ������ */
	alloc_notify_file(notify);
	if (!notify)
	{
		err = -ENOMEM;
		goto err;
	}

	notify->listener       = ke_current();
	notify->even_type_mask = cmd;
	notify->call_func      = func;
	notify->call_para      = para;

	/* 
		TODO:��ӵ�fss_file notify list�� 
		�Ƿ�Ҫ��ֹ�û���μ���ͬһ���ļ�ͬһ���¼�?
		�����Ҫ��֤,��ʱ������notify_list ��¼�û���
		��ʽҪ���¿���,���ܼ�ʹ������,��ʱ����Կ���
		ʹ��hash table,han key ����ʹ��PID����.
	*/
	ke_spin_lock(&file->notify_lock);
	list_add_tail(&notify->node, &file->notify_list);
	ke_spin_unlock(&file->notify_lock);
	
	err = 0;
err:
	return err;
}


/*
	@brief ȡ�������ļ��¼�����
	@return 
		= 0 �ɹ�  < 0 ���ش�����
*/
int fnotify_event_unregister(struct fss_file *file, y_file_event_type_t cmd)
{
	struct file_notify *notify;
	ke_thread lister;
	int err, find = 0;

	/* cmd�Ƿ�Ϸ�? */
	if (cmd & (~(Y_FILE_EVENT_READ | Y_FILE_EVENT_WRITE)))
	{
		err = -E2BIG;
		goto err;
	}
	
	ke_spin_lock(&file->notify_lock);
	
	/* ����notify list */
	lister = ke_current();
	list_for_each_entry(notify, &file->notify_list, node)
	{
		if ((notify->listener == lister) &&
			(cmd & notify->even_type_mask))
		{
			find = 1;
			break;
		}
	}
	
	/* �ҵ���ǰע�����Ϣ,�����û�Ҫ���ȫ�������¼� ?*/
	if (find && !(notify->even_type_mask &= ~cmd))
	{
		list_del(&notify->node);
		free_notify_file(notify);
	}
	
	ke_spin_unlock(&file->notify_lock);	
	
	err = 0;
err:
	return err;
}



