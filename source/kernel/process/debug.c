/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   线程调度器
 */

#include "thread.h"
#include "process.h"
#include "cpu.h"

#include "object.h"
void kp_debug_show_process(struct kc_cpu *cpu)
{
	struct ko_thread *thread;
	int i;
	
	for (i = 0; i < KT_PRIORITY_COUNT; i++)
	{
		printk("QueueID %d, run count %d:\n", i, cpu->run_count[i]);
		list_for_each_entry(thread, &cpu->run_queue[i], queue_list)
		{
			printk("name %s(%s), ", cl_object_get_name(thread), cl_object_get_name(thread->process));
		}
	}
	printk("\n");
}