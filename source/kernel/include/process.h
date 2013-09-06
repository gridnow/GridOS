#ifndef KP_PROCESS_H
#define KP_PROCESS_H

#include <types.h>
#include <list.h>

#include <walk.h>

#include <spinlock.h>

/* Process privilege level */
#define KP_CPL0						0
#define KP_CPL0_FAKE				1
#define KP_USER						3

#define KP_LOCK_PROCESS_SECTION_LIST(P) spin_lock(&(P)->vm_list_lock)
#define KP_UNLOCK_PROCESS_SECTION_LIST(P) spin_unlock(&(P)->vm_list_lock)
struct ko_process
{
	int cpl;
	
	struct km mem_ctx;
	struct ke_spinlock vm_list_lock;
	struct list_head vm_list;
};

/**
	@brief 释放memory Key
*/
void kp_put_mem(struct km *mem);

/**
	@brief 准备对address进行操作

	@return
		如果address没有别人在操作，那么返回成功，否则返回失败.
	@note
		一般来说，相同页的操作是只有一个被运行，另外一个等待。如果等待着出去后又发生相同的页异常，那么表示系统有问题。（TODO:侦测该情况）
*/
struct km *kp_get_mem(struct ko_process *who);

//process.c
struct ko_process *kp_get_system();

/**
	@brief 获取文件进程
*/
struct ko_process *kp_get_file_process();

/**
	@brief 创立一个空的进程对象
*/
struct ko_process *kp_create(int cpl, xstring name);

/**
	@brief 启动第一个用户态进程
 */
void ke_run_first_user_process(void *data, int size, char *cmdline);

//srv.c
void ke_srv_init();

#endif