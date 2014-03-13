/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   线程异步消息
 */

#ifndef KE_MESSAGE_H
#define KE_MESSAGE_H

#include <kernel/ke_lock.h>
#include <message.h>

//struct ko_thread;
struct ko_section;

struct ktm
{
	struct ko_section *map;
	
	struct y_message_instance desc;
	struct ke_spinlock lock;
	struct list_head waiting;
};

#define KE_MESSAGE_WAIT_STATUS_NONE			0
#define KE_MESSAGE_WAIT_STATUS_OK			1
#define KE_MESSAGE_WAIT_STATUS_ABANDONED	2
struct ke_message_wait
{
	struct list_head list;														//This node linked to the waiting list 
	struct y_message *sent;														//The message slot(s) from the dst thread
	struct ko_thread *who_sent;													//原发送者是谁，ACK的时候可以唤醒
	int wait_status;
};

//msg.c
void ktm_ack_sync(struct y_message *what);
void ktm_delete();
bool ktm_send(struct ko_thread *to, struct y_message *what);
struct ktm *ktm_prepare_loop();
void ktm_msg_init(struct ko_thread *who);
bool ktm_init();

#define KE_MESSAGE_SLOTS_BUFFER_SIZE (PAGE_SIZE * 16/*The bitmap will also need space, so normally a page is wasted for it*/)
#define KE_MESSAGE_SLOTS_SIZE_PER_THREAD (PAGE_SIZE)

#endif

