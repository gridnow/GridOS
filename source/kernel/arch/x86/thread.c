/**
	Thread support for x86
*/

#include <thread.h>
#include <process.h>
#include <arch/thread.h>

#include <asm/processor-flags.h>
#include <asm/segment.h>

/*
	�����THREAD_ENTRY��PARA����FATE_ENTRY,��������EFLAGS,һ��LongJMP to fate entry(ӦΪ���������û�����Ҳ�п������ں˼�
*/
static void __noreturn first_time_entry()
{
	struct ko_thread *who;	
	unsigned long _sp;
	unsigned long _ds,_cs;
	who = kt_arch_get_current();
	
	if (who->process->cpl != KP_USER)
	{
		_ds = __KERNEL_DS;
		_cs = __KERNEL_CS;
		_sp = who->arch_thread.ctx.sp0;
	}
	else
	{
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
		X86��ջ��: 
			ss
			esp
			eflags
			cs
			eip;
		64λ���� 
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
			   [EFLAGS]  "i" (ARCH_THREAD_ARCH_X86_32_DEFAULT_EFLAGS), 
			   [reg_cs]  "r" (_cs),
		       [reg_ip]  "r" (who->arch_thread.ctx.cr2),
		       
			   "d" (who->arch_thread.ctx.error_code),
			   "a" (who->arch_thread.ctx.ptrace_dr7)			
			   
		     : /* reloaded segment registers */		
			"memory");								
	while(1);
}

asmregparm struct ko_thread *arch_thread_switch(struct ko_thread *prev_p, struct ko_thread *next_p)
{
	struct tss_struct *tss = &init_tss;

	/*
	* Reload esp0.
	*/
	tss->x86_tss.sp0 = kt_arch_get_sp0(next_p);

	stts();

#if 0
	printk(".Real arch arch_thread_switch prev = %x, next = %x, next_sp = %x, pre sp = %x.\n",
		   prev_p, next_p,
		   next_p->arch_thread.ctx.sp,
		   prev_p->arch_thread.ctx.sp);
#endif
	return prev_p;
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
		thread->arch_thread.ctx.debugreg6	= (unsigned long)ctx->stack_pos;
	}

	/* Switch stack only recognize the sp */
	thread->arch_thread.ctx.sp =
		thread->arch_thread.ctx.sp0 = (unsigned long)ctx->stack0 + ctx->stack0_size;	
	/*
		���:
			error_code			= ��ڲ���;
			cr2					= fate_entry;
			ptrace_dr7			= thread_entry;	
			debugreg6			= stack top (if user thread)
	*/

	thread->arch_thread.ctx.ip				= (unsigned long)first_time_entry;
	thread->arch_thread.ctx.error_code		= ctx->para;
	thread->arch_thread.ctx.cr2				= (unsigned long)ctx->fate_entry;
	thread->arch_thread.ctx.ptrace_dr7		= (unsigned long)ctx->thread_entry;
}

static void dump_memory(unsigned long base, int size)
{
	int i = 0;
	unsigned char *p = (unsigned char*)base;
	
	while (size > 0)
	{
		printk("%08x:", p);
		for (i = 0; i < 16 && size > 0; i++, size--)
			printk("%02x ", *p++);
		printk("\n");
	}
}

void kt_arch_switch(struct ko_thread *prev, struct ko_thread *next)
{
	struct ko_thread * last;

	x86_thread_switch_to(prev, next, last);
	
	//printk("kt_arch_switch sp %x... %x \n", &last, __builtin_return_address(0));
}