
#include <fsnotify.h>
#include <kernel/ke_lock.h>
#include <kernel/ke_thread.h>
#include <node.h>
#include <message.h>
#include <kernel/ke_memory.h>
#include <errno.h>

#define DBGMSG(form, ...) printk(form , ##__VA_ARGS__)

/* 分配struct file_notify */
#define alloc_notify_file(fnotify) do{\
	fnotify = (struct file_notify *)km_valloc(sizeof(struct file_notify));\
	if (fnotify)\
	{\
		memset(fnotify, 0, sizeof(struct file_notify));\
		INIT_LIST_HEAD(&fnotify->node);\
	}\
	}while(0)

/* 释放 file notify */
#define free_notify_file(fnotify) do{\
	km_vfree(fnotify);\
	fnotify = NULL;\
	}while(0)


/*
	@brief 向文件监听者发送事件消息
	@return 
		无
*/
void fnotify_msg_send(struct fss_file *file, y_file_event_type_t even)
{
	struct file_notify *ntf_file = NULL;

	ke_spin_lock(&file->notify_lock);
	
	/* 向所有监听进程发送消息 */
	list_for_each_entry(ntf_file, &file->notify_list, node)
	{
		/* 是否为监听敢兴趣事件 ? */
		if (ntf_file->even_type_mask & even)
		{
			/* 构造发送消息 */
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
	@brief 供上层调用实现注册监听文件
	@return 
		= 0 成功  < 0 返回错误码
*/
int  fnotify_event_register(struct fss_file *file, y_file_event_type_t cmd, void *func, void *para)
{
	struct file_notify *notify;
	int err;
	
	/* cmd是否合法? */
	if (cmd & (~(Y_FILE_EVENT_READ | Y_FILE_EVENT_WRITE)))
	{
		err = -E2BIG;
		goto err;
	}

	/* 分配,构造notify.TODO 需要缓冲区 */
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
		TODO:添加到fss_file notify list中 
		是否要防止用户多次监听同一个文件同一个事件?
		如果需要保证,这时候我们notify_list 记录用户的
		方式要重新考虑,不能简单使用链表,这时候可以考虑
		使用hash table,han key 可以使用PID计算.
	*/
	ke_spin_lock(&file->notify_lock);
	list_add_tail(&notify->node, &file->notify_list);
	ke_spin_unlock(&file->notify_lock);
	
	err = 0;
err:
	return err;
}


/*
	@brief 取消监听文件事件类型
	@return 
		= 0 成功  < 0 返回错误码
*/
int fnotify_event_unregister(struct fss_file *file, y_file_event_type_t cmd)
{
	struct file_notify *notify;
	ke_thread lister;
	int err, find = 0;

	/* cmd是否合法? */
	if (cmd & (~(Y_FILE_EVENT_READ | Y_FILE_EVENT_WRITE)))
	{
		err = -E2BIG;
		goto err;
	}
	
	ke_spin_lock(&file->notify_lock);
	
	/* 遍历notify list */
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
	
	/* 找到以前注册的信息,并且用户要清除全部监听事件 ?*/
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



