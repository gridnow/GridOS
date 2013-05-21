/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ARM ƽ̨�߳�
 *
 */

#include <thread.h>
#include <process.h>
#include <arch/thread.h>

/*
	�����THREAD_ENTRY��PARA����FATE_ENTRY,��������EFLAGS,һ��LongJMP to fate entry(ӦΪ���������û�����Ҳ�п������ں˼�
*/
static void __noreturn first_time_entry()
{
	struct ko_thread *who;
	unsigned long _sp;
	unsigned long _ds,_cs;
	who = kt_arch_get_current();
	
	if (who->process->cpl == KP_CPL0)
	{
		
		printk("Ring 0 stack is %x.\n", _sp);
	}
	else
	{
		printk("\nring 3...\n");
		
	}
	

	while(1);
}

asmregparm void arch_thread_switch(struct ko_thread *prev_p, struct ko_thread *next_p)
{
	
}

/**
 @brief Create the arch thread
 */
void kt_arch_init_thread(struct ko_thread * thread, struct kt_thread_creating_context * ctx)
{
	/*
		CPL0 Stack ,if cpl0, more stack and no user stack,
		otherwise little stack.
	*/
	if (ctx->cpl == KP_USER)
	{
		/* ��ͨ��ջ */
	}
	
	/* Switch stack only recognize the sp */
	//thread->arch_thread.ctx.sp =
	//thread->arch_thread.ctx.sp0 = (unsigned long)ctx->stack0 + ctx->stack0_size;
	/*
	 ���:
		error_code			= ��ڲ���;
		cr2					= fate_entry;
		ptrace_dr7			= thread_entry;
		debugreg6			= stack top (if user thread)
	*/
	
}

void kt_arch_switch(struct ko_thread * prev, struct ko_thread * next)
{
	
}

