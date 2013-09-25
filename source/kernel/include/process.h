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
	
	/* Handle table */
	void *handle_table;
	spinlock_t handle_lock;
};

/**
	@brief �ͷ�memory Key
*/
void kp_put_mem(struct km *mem);

/**
	@brief ׼����address���в���

	@return
		���addressû�б����ڲ�������ô���سɹ������򷵻�ʧ��.
	@note
		һ����˵����ͬҳ�Ĳ�����ֻ��һ�������У�����һ���ȴ�������ȴ��ų�ȥ���ַ�����ͬ��ҳ�쳣����ô��ʾϵͳ�����⡣��TODO:���������
*/
struct km *kp_get_mem(struct ko_process *who);

//process.c
struct ko_process *kp_get_system();

/**
	@brief ��ȡ�ļ�����
*/
struct ko_process *kp_get_file_process();

/**
	@brief ����һ���յĽ��̶���
*/
struct ko_process *kp_create(int cpl, xstring name);

/**
	@brief ������һ���û�̬����
 */
void ke_run_first_user_process(void *data, int size, char *cmdline);

//srv.c
void ke_srv_init();

#endif