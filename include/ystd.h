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

BEGIN_C_DECLS;

/* Should move to kernel standard typedef */
typedef unsigned long y_handle;
#define Y_INVALID_HANDLE (-1UL)

/************************************************************************/
/* 进程                                                                         */
/************************************************************************/
typedef enum 
{
	Y_SYNC_WAIT_RESULT_OK	= 0,
	Y_SYNC_WAIT_ABANDONED	= -1,
	Y_SYNC_WAIT_TIMEDOUT	= -2,
	Y_SYNC_WAIT_ERROR		= -3,
} y_wait_result;

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
*/
y_wait_result y_process_wait_exit(y_handle for_who, unsigned long * __in __out result);

/************************************************************************/
/* 同步对象                                                                     */
/************************************************************************/
#define Y_SYNC_MAX_OBJS_COUNT 64
#define Y_SYNC_WAIT_INFINITE -1 

typedef enum {
	Y_MSG_LOOP_EXIT_SIGNAL = 0,
	Y_MSG_LOOP_ERROR = -1,
}y_msg_loop_result;

/**
	@brief 等待线程消息
*/
y_msg_loop_result y_message_loop();

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

/**
	@brief 打开文件

	系统原生接口，目前还木有具体完善，因此参数
	就一个

	@return
		返回文件对象句柄
*/
y_handle y_file_open(const char *path);

/**
	@brief 读取文件

	读取打开成功的文件到buffer 中

	@param[in] file 要读取的文件
	@param[in] size 读取长度
	@param[in] size 要写入的缓冲区，用户保证缓冲区长度起码有	size长度 

	@return
		>0 表示成功读取的字节数，不会超过过size,
		<0表示错误码.
*/	
ssize_t y_file_read(y_handle file, size_t size, void *buffer);

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
