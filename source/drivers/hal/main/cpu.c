/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPU 描述
*/
#include "thread.h"
#include "memory.h"
#include "priority.h"
#include "cpu.h"

/* 用PERCPU的话，该变量将被删掉*/
struct kc_cpu tmp_boot_cpu;

static struct kc_cpu * this_cpu = &tmp_boot_cpu;
static struct ko_thread dummy_kmt = {1}/*Use DATA segment, 否则占用过多的BSS，防止其他模块破坏内核*/;

static void dumy_idle_ops(int type)
{
#ifdef __i386__
	__asm volatile("hlt\n");
#endif
}

static void init_a_cpu(struct kc_cpu * cpu)
{
	int i;
	struct ke_mem_cluster_info info = {0};

	for (i = 0; i < KT_PRIORITY_COUNT; i++)
		INIT_LIST_HEAD(&cpu->run_queue[i]);
	INIT_LIST_HEAD(&cpu->dieing_queue);

	/* Idle routine */
	cpu->idle_ops = dumy_idle_ops;

	/* 安装一个假的THREAD到CPU上，应为在启动阶段我们用CURRENT's preemnt来分配页（防止被抢占的）*/
	cpu->cur = &dummy_kmt;

	/* Give the cpu a memory cluster */
	km_cluster_alloc(&info, 0, false);
	cpu->mm_current_cluster = info.km_cluster;

	/* Its own pointer for accelerated access */
	cpu->self = cpu;

	/* Now set an ARCH way to get the cpu environment (km_cpu) */
	km_setup_percpu(cpu->id, (unsigned long)cpu);
}

void kc_init()
{
	init_a_cpu(this_cpu);
}

