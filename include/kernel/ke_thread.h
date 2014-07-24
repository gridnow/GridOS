/**
   @defgroup thread
   @ingroup kernel_api
	
   内核编程接口 读写锁

   @{
*/
#ifndef KE_THREAD_H
#define KE_THREAD_H

#include <types.h>

typedef unsigned long ke_thread;
struct y_message;

/**
	@brief 获取当前进程,供内核编程使用
*/
ke_thread ke_current();

/**
	@brief 创建内核线程
*/
ke_thread ke_create_kernel(void *entry, void *para);

/**
	@brief 提供给内核编程接口使用
*/
bool ke_msg_send(ke_thread to, struct y_message *what);

#endif
/** @} */

