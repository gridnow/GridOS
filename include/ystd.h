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

typedef enum 
{
	Y_SYNC_WAIT_RESULT_OK	= 0,
	Y_SYNC_WAIT_ABANDONED	= -1,
	Y_SYNC_WAIT_TIMEDOUT	= -2,
	Y_SYNC_WAIT_ERROR		= -3,
}y_wait_result;


/*
	文件以及目录改变事件类型
*/
typedef enum{
	Y_FILE_EVENT_READ  = (1 << 0),
	Y_FILE_EVENT_WRITE = (1 << 1)
}y_file_event_type_t;

/************************************************************************/
/* 进程                                                                         */
/************************************************************************/
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
