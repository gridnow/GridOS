/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   œﬂ≥Ã√Ë ˆ
*/

#ifndef KT_THREAD_H
#define KT_THREAD_H

#include "object.h"
#include <arch/thread.h>

struct ko_process;
struct ko_thread
{
	struct kt_arch_thread arch_thread;
	struct cl_object object;
	struct ko_process *process;
};

/* Context for thread creating */
struct kt_thread_creating_context
{
	struct ko_process *on;
	unsigned long stack_pos;
	unsigned long stack0, stack0_size;
	unsigned long fate_entry;
	unsigned long thread_entry;
	int priority;
	unsigned long para;
	int cpl;
};

#define KT_CREATE_STACK_AS_PARA (1 << 1)
#endif

