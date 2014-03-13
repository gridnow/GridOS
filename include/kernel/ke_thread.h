
#ifndef KE_THREAD_H
#define KE_THREAD_H

typedef unsigned long ke_thread;

/*
	@brief 获取当前进程,供内核编程使用
*/
struct ko_thread *ke_current();

/*
	@brief 提供给内核编程接口使用
*/
bool ke_send(struct ko_thread *to, struct y_message *what);

#endif


