/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 */

/**
*  @defgroup y_standard
*  @ingroup Y_Standard_Headers
*
*
*
*  @{
*/
#ifndef _Y_STANDARD_H
#define _Y_STANDARD_H

#include <compiler.h>
#include <types.h>
#include <kernel/kernel.h>
#include <kernel/ke_srv.h>

BEGIN_C_DECLS;

/************************************************************************/
/* 进程                                                                  */
/************************************************************************/
struct y_thread_environment_block
{
	void *self;
	
	/* Message information */
	void *mi;

	/* Pthread specific key/value */
	void *pt_speci;
};

typedef unsigned long message_id_t;
struct y_message
{
	unsigned short count;
	volatile unsigned short flags;
	message_id_t what;
};
typedef void (*y_message_func)(struct y_message *msg);

#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE -1
typedef kt_sync_wait_result y_wait_result;
typedef enum {
	Y_MSG_LOOP_EXIT_SIGNAL = 0,
	Y_MSG_LOOP_ERROR = -1,
} y_msg_loop_result;

/**
	@brief 创立进程

	@param[in] name 进程名
	@param[in] cmdline 启动命令行

	@return
		成功返回进程句柄，失败返回Y_INVALID_HANDLE
*/
y_handle y_process_create(xstring name, char *cmdline);

/**
	@brief 等待进程退出
	
	@param[in] for_who 要等待的进程对象
	@param[in][out] 可以为NULL，否则回写进程for_who的退出结果（一般是main函数的返回值）
 
	@return
		See y_wait_result.
*/
y_wait_result y_process_wait_exit(y_handle for_who, unsigned long * __in __out result);

/**
	@brief 读取消息

	消息处理触发函数被调用外，一般还可以携带数据

	@param[in] what 消息对象，用户实际上不直接访问这个结构中的内容

	@note:
		该函数的用法举例:
		y_message_read(msg, &arg0, &arg1);
		其中arg0 和argv1 是unsigned long类型
	@return
		The count of parameters the message attached.
*/
int y_message_read(struct y_message *what, ...);

/**
	@brief 写入返回消息
 
	一般用户为了同步消息的返回值，需要写入消息。前提本次写入不能超过对方发送时消息携带的参数个数。
 
	@param[in] what 消息对象，用户实际上不直接访问这个结构中的内容
	@param[in] wb_count 回写参数个数，不可多于发送时的数量
 
	@return
		The count of parameters written, < 0 for error!
*/
int y_message_writeback(struct y_message *what, int wb_count, ...);

/**
	@brief 等待线程消息
*/
y_msg_loop_result y_message_loop();

/**
	@brief 等待事件的发生
 
	线程阻塞等待事件的出发，如果一个事件曾经被出发过，但是是还没有被还原，那么等待失效，直到重新被恢复。
 
	@param[in] event 等待的事件对象
	@param[in] timeout 等待超时，单位为ms，Y_SYNC_WAIT_INFINITE 为无限等待
 
	@return
		The result in the format of y_wait_result
*/
y_wait_result y_event_wait(y_handle event, int timeout);

/**
	@brief 发送消息到线程
*/
bool y_message_send(ke_handle to_thread, struct y_message *what);

/**
	@brief 注册消息处理函数
*/
bool y_message_register(message_id_t message_id, y_message_func call_back_func);

/**
	@brief 触发一个事件

	唤醒事件的等待者
 
	@param[in] event 要触发的事件
 
	@return
		唤醒了几个线程，< 0表示错误
*/
int y_event_set(y_handle event);

/**
	@brief 创建事件对象
	
	创建事件同步对象，并设置事件的类型。
 
	@param[in] manual_reset 是否是自动还原的事件，如果是那么被触发后会唤醒一个线程，并设置为未触发状态，否则需要手动处理
	@param[in] initial_status 事件原始状态是触发的还是没有被触发的
 
	@return
		成功返回事件对象，失败返回Y_INVALID_HANDLE.
*/
y_handle y_event_create(bool manual_reset, bool initial_status);

/**
	@brief 删除事件
 
	删除事件对象，如果事件对象有人在等待，那么这些线程都将被唤醒，并返回取消错误码。
*/
void y_event_delete(y_handle event);

/************************************************************************/
/* 文件对象                                                                     */
/************************************************************************/
/*
	@brief 文件以及目录改变事件类型
*/
typedef enum{
	Y_FILE_EVENT_READ  = (1 << 0),
	Y_FILE_EVENT_WRITE = (1 << 1)
} y_file_event_type_t;

/*
	@brief 打开文件初始化文件操作类型
*/
typedef enum{
	Y_FILE_OPERATION_CACHE = 1,
	Y_FILE_OPERATION_NOCACHE
} y_file_operation_type_t;

/**
	@brief 打开文件

	系统原生接口，目前还木有具体完善，因此参数
	就一个

	@return
		返回文件对象句柄
*/
y_handle y_file_open(const char *path, y_file_operation_type_t ops_type);

/**
	@brief 读取文件

	读取打开成功的文件到buffer 中

	@param[in] file 要读取的文件
	@param[in] buffer 要写入的缓冲区，用户保证缓冲区长度起码有	size长度 	
	@param[in] size 读取长度

	@return
		>0 表示成功读取的字节数，不会超过过size,
		<0表示错误码.
*/	
ssize_t y_file_read(y_handle file, void *buffer, size_t size);

/**
	@brief 写入文件

	把用户指定buffer 写入打开成功的文件

	@param[in] file 要写入的文件
	@param[in] buffer 源缓冲区，用户保证缓冲区长度起码有	size长度 	
	@param[in] size 写入长度

	@return
		>0 表示成功读取的字节数，不会超过过size,
		<0表示错误码.
*/	
ssize_t y_file_write(y_handle file, void *buffer, size_t size);

/**
	@brief Seek file

*/
int y_file_seek(y_handle file, loff_t where, int whence);

/**
	@brief 关闭文件

	关闭打开的y_handle 文件
*/
void y_file_close(y_handle file);


/**
	@brief 注册文件监听事件

	当文件的某个被监听的事件被其他进程触发，那么
	用户通过本函数注册的回调函数将在消息循环的时候被
	调用，因此用户需要准备一个消息循环的线程来处理
	该回调，一般是第一个线程来做消息循环。

	@param[in] file 要监听哪个文件
	@param[in] event_mask 事件类型，可以多个组合，参见y_file_event_type_t
	@param[in] func 事件发生时的回调函数
	@param[in] para 事件发生时回调函数的参数
	
	@return
		等于0正常，<0 则为错误码。
*/
int y_file_event_register(y_handle file, y_file_event_type_t event_mask, void *func, void *para);

/**
	@brief 取消监听某个文件的事件

	如果用户以前建听过某个文件的某些事件，那么可以通过
	本接口取消关心这些事件，从而就不会有回调消息被发送该本进程的消息循环上。
	如果要被取消的事件以前没有注册，那么本函数返回错误码。

	
	@param[in] file 要监听哪个文件
	@param[in] event_mask 事件类型，可以多个组合，参见y_file_event_type_t

	@return
		等于0正常，<0 则为错误码。
*/
int y_file_event_unregister(y_handle file, y_file_event_type_t event_mask);

/**
	@brief 将文件映射在用户空间虚拟内存
	
	若用户要像使用内存访问一样访问一个文件,可以使用该方法将文件
	映射入用户虚拟内存空间

	@param[in] file 要映射的文件
	@param[in] len  映射的长度
	@param[in] prot  映射的页面权限
	@param[in] flags The  flags argument determines whether updates to the mapping are visi-
					ble to other processes mapping the same region, and whether updates are
					carried through to the underlying file.
	@param[in] offset 从文件偏移长度开始映射

	@return
		成功返回映射到用户虚拟空间的地址,失败返回NULL
*/
void * y_file_mmap(y_handle file, size_t len, page_prot_t prot, int flags, off_t offset);



/************************************************************************/
/* CONSOLE                                                              */
/************************************************************************/
void sys_set_pixel(int x, int y, unsigned int color);
void sys_draw_screen(int x, int y, int width, int height, int bpp, void * bitmap_buffer);
void sys_get_screen_resolution(int *width, int * height, int *bpp);

/**
	@brief 操作CMDLINE
*/
int crt0_split_cmdline(char * cmdline, int max_size, int *argc, int max_argc, char **argv);

END_C_DECLS;

#endif /* _Y_STANDARD_H */

/** @} */
