#ifndef KP_PROCESS_H
#define KP_PROCESS_H

#include <types.h>
#include <list.h>

#include <walk.h>

#include <spinlock.h>

/* Process privilege level */
#define KP_CPL0						0
#define KP_USER						3

struct ko_process
{
	int cpl;
	
	struct km mem_ctx;
	struct ke_spinlock vm_list_lock;
	struct list_head vm_list;
};

//process.c
struct ko_process *kp_get_system();

struct ko_process *kp_create(int cpl, xstring name);

#endif