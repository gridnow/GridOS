/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPU 描述
*/

#ifndef KC_H
#define KC_H
#include <list.h>
#include "priority.h"

struct ko_thread;
struct kc_cpu
{
	/* cur thread */
	struct ko_thread * cur;

	/* CPU itself, 由于有了汇编器，该指针有了特别的意思，哈哈 */
	struct km_cpu * self;
	int id;

	/* Thread queue */
	struct list_head run_queue[KT_PRIORITY_COUNT];
	unsigned int run_count[KT_PRIORITY_COUNT];
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

/* 用PERCPU的话，该变量将被删掉*/
extern struct kc_cpu tmp_boot_cpu;
static inline struct kc_cpu *kac_get()
{
	/* TODO:call the arch to get current cpu base */
	return &tmp_boot_cpu;
}

static inline struct kc_cpu *kc_get()
{
	//TODO:preempt disable
	struct kc_cpu *cpu = kac_get();
	return cpu;
}

static inline void kc_put()
{
	//TODO:preempt enable
}

#define kc_get_raw()		(kac_get())


#endif
