/**
*  @defgroup posix_unistd
*  @ingroup 标准C库编程接口
*
*  posix unistd
*
*  @{
*/
#ifndef _UNISTD_H
#define _UNISTD_H

#include <types.h>

BEGIN_C_DECLS;

int ftruncate(int fd, off_t length);

/**
	@brief Sleep for the specified number of seconds

	Makes the calling process sleep until seconds seconds have elapsed or a signal arrives which is not ignored.

	@return
		Zero if the requested time has elapsed, or the number of seconds left to sleep, if the call was interrupted by a signal handler.
*/
unsigned int sleep(unsigned int seconds);

/** 
	@brief 切换当前线程工作目录

	切换目录的操作只影响当前线程，即便是在同一个进程不影响其他线程。

	@param[in] path 要切换的目录名，可以是相对路径（相对当前线程工作的路径），也可以绝对路径，”..”表示上一级路径。

	@return
		成功返回0，失败返回-1。
*/
int chdir(const char *path);

END_C_DECLS;

#endif

/** @} */