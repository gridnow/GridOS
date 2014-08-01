/*
	threads for mips
	wuxin
*/

#include <process.h>
#include <thread.h>
#include <memory.h>
#include <cpu.h>

#include <asm/mipsregs.h>

extern void start_new_thread();
extern void switch_thread_stack(struct ko_thread * prev, struct ko_thread * next, unsigned long next_sp0);
unsigned long *cpu_base_array[NR_CPUS];

/**
	@brief Create the arch thread
*/
void kt_arch_init_thread(struct ko_thread * thread, struct kt_thread_creating_context * ctx)
{
	unsigned long status_set = 0;
	unsigned long old;
	
	/* Create the thread from micro layer */
	if (!ctx->thread_entry)
	{
		hal_printf_warning("创立线程时为何不给入口地址？");
	}

	/* 
		CPL0 Stack ,if cpl0, more stack and no user stack, 
		otherwise little stack.
	*/
	if (ctx->cpl == KP_USER)
	{
		status_set = KSU_USER;
	}
	else
	{
		status_set = ST0_CU0;
 		ctx->stack_pos = (unsigned long)ctx->stack0 + ctx->stack0_size;
	}
	
	/* 
		The new thread is started by "switch_thread_stack -> start_new_thread".
		So setup the context FOR switch_stack 29(sp) and 31(ra) returns to start_new_thread.
		Then, by eret to the user thread.so (ST0_EXL)。
	*/	
	thread->arch_thread.ctx.reg16 = (unsigned long)ctx->fate_entry;
	thread->arch_thread.ctx.reg17 = (unsigned long)ctx->thread_entry;
	thread->arch_thread.ctx.reg18 = (unsigned long)ctx->para;
	thread->arch_thread.ctx.reg19 = (unsigned long)ctx->stack_pos;
	thread->arch_thread.ctx.reg29 = (unsigned long)ctx->stack0 + ctx->stack0_size;
	thread->arch_thread.ctx.sp0	  = (unsigned long)thread->arch_thread.ctx.reg29;
	thread->arch_thread.ctx.reg31 = (unsigned long)start_new_thread;
#ifdef CONFIG_64BIT
	status_set |= ST0_FR|ST0_KX|ST0_SX|ST0_UX;
#endif
	old = read_c0_status();
	status_set |= ST0_EXL | ST0_IE | (old & ST0_IM); 
	thread->arch_thread.ctx.cp0_status = status_set;
	
}

/**
	@brief switch a thread 
*/
void kt_arch_switch(struct ko_thread * prev, struct ko_thread * next)
{
	//printk("next %s\n", (char*)(long)kmt_get_name((next)));	
	switch_thread_stack(prev, next, next->arch_thread.ctx.sp0);
  	
}

/**
	@brief An arch way to get the CPU environment
*/
void hal_arch_setup_percpu(int cpu, unsigned long base)
{
	cpu_base_array[cpu] = (unsigned long *)base ;
}
