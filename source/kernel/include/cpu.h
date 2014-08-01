/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPU ����
*/

#ifndef KC_H
#define KC_H
#include <list.h>
#include "preempt.h"
#include "priority.h"
#include <arch/page.h>

struct kc_arch_cpu
{
#ifdef __mips__
	unsigned long asid, cur_asid;
	void *asid_confict_table[CPU_PAGE_FALG_ASID_MASK + 1];
#endif
};

struct ko_thread;
struct kc_cpu
{
	/* cur thread */
	struct ko_thread * cur;

	/* CPU itself, �������˻��������ָ�������ر����˼������ */
	struct kc_cpu * self;
	struct kc_arch_cpu cpu;
	int id;

	/* Thread queue */
	struct list_head run_queue[KT_PRIORITY_COUNT];
	unsigned int run_count[KT_PRIORITY_COUNT];
	struct ko_thread *driver_thread;
	unsigned long run_mask;
	unsigned long running_count;

	/* Dieing thread */
	struct list_head dieing_queue;

	/* About idle */
	struct ko_thread * idle;
	void (*idle_ops)(int type);

	/* Current memory cluster */
	struct km_cluster * mm_current_cluster;
};

/* ��PERCPU�Ļ����ñ�������ɾ��*/
extern struct kc_cpu tmp_boot_cpu;
static inline struct kc_cpu *kac_get()
{
	/* TODO:call the arch to get current cpu base */
	return &tmp_boot_cpu;
}

static inline struct kc_cpu *kc_get_raw()
{
	return kac_get();
}

static inline struct kc_cpu *kc_get()
{
	hal_preempt_disable();
	return kc_get_raw();
}

static inline void kc_put()
{
	hal_preempt_enable();
}

#endif
