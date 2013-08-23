#ifndef KP_PROCESS_H
#define KP_PROCESS_H

#include <types.h>
#include <list.h>

#include <walk.h>

#include <spinlock.h>

/* Process privilege level */
#define KP_CPL0						0
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

static inline struct km *kp_get_mem(struct ko_process *who)
{
	struct km *mem = &who->mem_ctx;
	spin_lock(&mem->lock);
	return mem;
}

static inline void kp_put_mem(struct km *mem)
{
	spin_unlock(&mem->lock);
}

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
void ke_run_first_user_process(void *data, int size);


#endif