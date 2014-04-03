
#ifndef FSNOTIFY_H
#define FSNOTIFY_H

#include <vfs.h>
#include <ystd.h>
#include <kernel/ke_srv.h>
#include <kernel/ke_thread.h>



/*
	文件通告记录数据结构,内嵌在fss_file结构内监听链内
*/
struct file_notify
{
	/* 用于链接fs_file的监听链 */
	struct list_head node;
	/* 监听线程 */
	ke_thread  listener;
	/* 监听事件类型 */
	y_file_event_type_t even_type_mask;
	
	/* 注册回调函数与参数 */
	void *call_func;
	void *call_para;
};

/*
	文件通告消息格式
*/
struct y_file_notify_msg
{
	/* 消息类型 */
	y_file_event_type_t even_mask;
	/* 注册回调函数与参数 */
	void *call_func;
	void *call_para;
};

/**
	@ brief 文件通告函数,供文件发生改变调用
	@return 
		void 
*/

void fnotify_msg_send(struct fss_file *file, y_file_event_type_t even_mask);


/**
	@brief 提供用户注册文件通告事件
	@return
		成功 = 0, < 0 返回错误码
*/
int  fnotify_event_register(struct fss_file *file, y_file_event_type_t cmd, void *func, void *para);

/**
	@brief 取消监听文件事件类型
	@return
		成功 = 0, < 0 返回错误码
*/
int fnotify_event_unregister(struct fss_file *file, y_file_event_type_t cmd);

#endif


