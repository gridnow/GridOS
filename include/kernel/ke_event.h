/**
	@defgroup Event
	@ingroup kernel_api
 
	内核编程接口 Event
 
 @{
*/

#ifndef KE_EVENT_H
#define KE_EVENT_H
#include <types.h>
#include <compiler.h>
#include "kernel.h"

struct ke_event
{
	unsigned long reserved[KE_SYNC_BASE_OBJECT_RESERVE];										/* Room for kernel sync ops */
	char			initial;
	char			manual_reset;
	int				status;
};

/**
	@brief 初始化一个事件

	事件在被使用前要先初始化。

	@param[in] event 事件的有效指针
	@param[in] manual_reset 事件是否是手动复位的
	@param[in] initial_status 事件的初始状态是否是触发状态，如果是，那么第一个等待着将被唤醒
*/
void ke_event_init(struct ke_event *event, bool manual_reset, bool initial_status);

/**
	@brief 触发一个事件

	如果有线程正在等待事件的发生，那么触发事件将导致其他线程被唤醒，从而起到消息通知的作用。
	如果该事件是手动复位的，那么所有等待的线程将都被唤醒。
	如果该事件是自动复位的，那么只唤醒等待队列中最前面的线程。

	@param[in] event 事件的有效指针
*/
void ke_event_set(struct ke_event *event);

/**
	@brief 复位一个事件

	事件如果是手动复位的，那么当发生事件后后，要对其复位才能继续触发新的事件，
	否则它会一直处于触发状态，接收不到新的事件的到来。
	
	@param[in] event 事件的有效指针
*/
void ke_event_reset(struct ke_event *event);

/**
	@brief 等待一个事件发生

	阻塞等待一个事件发生。

	@param[in] event 事件的有效指针
	@param[in] timeout 超时单位为1/1000秒，如果为Y_SYNC_WAIT_INFINITE，则表示永不超时

	@return
		等待结果
*/
kt_sync_wait_result ke_event_wait(struct ke_event * event, unsigned int timeout);

/**
	@brief 等待一些列的事件发生

	阻塞等待一组事件发生。

	@param[in] count 事件数量
	@param[in] events 事件的有效指针数组
	@param[in] wait_all 是否要等待所有事件都被触发后才退出
	@param[in] timeout 超时单位为1/1000秒，如果为Y_SYNC_WAIT_INFINITE，则表示永不超时
	@param[in,out] id 第一个被触发的事件的id

	@return
		第一个发生Event的事件对象ID和等待结果
*/
kt_sync_wait_result ke_events_wait(int count, struct ke_event * events[], bool wait_all, unsigned int timeout, int *id);

#endif
/** @} */