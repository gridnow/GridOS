/**
	Thread support for x86
*/

#include <thread.h>
#include <process.h>
#include <arch/thread.h>

#include <asm/processor-flags.h>
#include <asm/segment.h>

/*
	负责把THREAD_ENTRY和PARA传入FATE_ENTRY,并且设置EFLAGS,一个LongJMP to fate entry(应为可能是在用户级，也有可能在内核级
*/
static void __attribute__((noreturn)) first_time_entry()  
{
	struct ko_thread *who;	
	unsigned long _sp;
	unsigned long _ds,_cs;
	who = kt_arch_get_current();
	
	if (who->process->cpl == KP_CPL0)
	{
		_ds = __KERNEL_DS;
		_cs = __KERNEL_CS;
		_sp = who->arch_thread.ctx.sp0;
		printk("Ring 0 stack is %x.\n", _sp);
	}
	else
	{
		printk("\nring 3...who->arch_thread.ctx.cr2 :%h.\n",who->arch_thread.ctx.cr2);
		_ds = __USER_DS;
		_cs = __USER_CS;
		_sp = who->arch_thread.ctx.debugreg6;
		asm volatile(
			 "movl %0,%%eax\n\t"	\
			 "movl %%eax,%%ds\n\t"	\
			 "movl %%eax,%%es\n\t"	\
			 "xorl %%eax,%%eax\n\t" \
			 "movl %%eax,%%fs\n\t"	\
			 "movl %%eax,%%gs\n\t"	\
			 ::
			 "m" (_ds)
			 : "eax", "memory");			
		
	}

	/*
		X86堆栈是: 
			ss
			esp
			eflags
			cs
			eip;
		64位相似 
	*/ 
	__asm volatile(
			 "pushl 	%[reg_ss]\n\t"	
			 "pushl 	%[reg_sp]\n\t"	
		     "pushl		%[EFLAGS]\n\t"
		     "pushl 	%[reg_cs]\n\t"	
		     "pushl 	%[reg_ip]\n\t"	
			 "iret \n\t"
		     													
			 : 
			 /* input parameters: */						
		     : [reg_ss]  "r" (_ds),
			   [reg_sp]  "r" (_sp), 
			   [EFLAGS]  "i" (ARCH_THREAD_ARCH_X86_32_DEFAULT_EFLAGS & (~X86_EFLAGS_IF)), 
			   [reg_cs]  "r" (_cs),
		       [reg_ip]  "r" (who->arch_thread.ctx.cr2),
		       
			   "d" (who->arch_thread.ctx.error_code),
			   "a" (who->arch_thread.ctx.ptrace_dr7)			
			   
		     : /* reloaded segment registers */		
			"memory");								
	while(1);
}

asmregparm void arch_thread_switch(struct ko_thread *prev_p, struct ko_thread *next_p)
{
	stts();
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
		/* 普通堆栈 */
		thread->arch_thread.ctx.debugreg6	= (unsigned long)ctx->stack_pos;
	}

	/* Switch stack only recognize the sp */
	thread->arch_thread.ctx.sp =
		thread->arch_thread.ctx.sp0 = (unsigned long)ctx->stack0 + ctx->stack0_size;	
	/*
		入口:
			error_code			= 入口参数;
			cr2					= fate_entry;
			ptrace_dr7			= thread_entry;	
			debugreg6			= stack top (if user thread)
	*/

	thread->arch_thread.ctx.ip				= (unsigned long)first_time_entry;
	thread->arch_thread.ctx.error_code		= ctx->para;
	thread->arch_thread.ctx.cr2				= (unsigned long)ctx->fate_entry;
	thread->arch_thread.ctx.ptrace_dr7		= (unsigned long)ctx->thread_entry;
}

void kt_arch_switch(struct ko_thread * prev, struct ko_thread * next)
{ 
	x86_thread_switch_to(prev, next);
}

