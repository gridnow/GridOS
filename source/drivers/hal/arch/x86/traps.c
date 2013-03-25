/*
*   X86 异常/中断挂接入口
*/
#include <types.h>

#include <asm/desc.h>
#include <asm/traps.h>
#include <asm/irq_vectors.h>
#include <asm/processor-flags.h>
#include <asm/irqflags.h>

#define DEBUG_PREFIX "Traps:"
DECLARE_BITMAP(used_vectors, NR_VECTORS);
/* Do we ignore FPU interrupts ? */
char ignore_fpu_irq;

void fak_thread_want_die()
{
	printk("TODO: Thread is die...\n");
	while(1);
}
void fak_arch_x86_dump_register(struct pt_regs *regs)
{
#define STACK_DUMP_COUNT 20
	int i;
	unsigned long *sp = (unsigned long*)regs->sp;
	printk("\n寄存器：EAX=%h,EBX=%h,ECX=%h,EDX=%h,ESI=%h,EDI=%h,EIP=%h,ESP=%h,EBP=%h.",
		regs->ax,regs->bx,regs->cx,regs->dx,regs->si,regs->di,regs->ip,regs->sp,regs->bp);

#if 0
	printk("\n堆栈%d个字节的内容:" ,STACK_DUMP_COUNT*sizeof(unsigned long));
	for(i=0;i<STACK_DUMP_COUNT;i++)
	{
		if((i%6)==0)
			printk("\n    ");
		printk("%h(%h) ",sp[i],i);
	}
	printk("\n  \n");
#endif
}

/*
	X86异常
*/
#define DO_ERROR(trapnr, signr, str, name)				\
	asmregparm void _do_##name(struct pt_regs *regs, long error_code)	\
{									\
	printk(DEBUG_PREFIX"%s,程序无法运行,异常号%d.",str,trapnr);	\
	fak_arch_x86_dump_register(regs);		\
	fak_thread_want_die();			\
}

#define DO_ERROR_INFO(trapnr, signr, str, name, sicode, siaddr)		\
	asmregparm void _do_##name(struct pt_regs *regs, long error_code)	\
{									\
	printk(DEBUG_PREFIX"%s,程序无法运行,异常号%d.",str,trapnr);	\
	fak_arch_x86_dump_register(regs);	\
	fak_thread_want_die();			\
}
DO_ERROR_INFO(0, SIGFPE, "divide error", divide_error, FPE_INTDIV, regs->ip)
DO_ERROR(4, SIGSEGV, "overflow", overflow)
DO_ERROR(5, SIGSEGV, "bounds", bounds)
DO_ERROR_INFO(6, SIGILL, "invalid opcode", invalid_op, ILL_ILLOPN, regs->ip)
DO_ERROR(8, SIGSEGV, "double fault", double_fault)
DO_ERROR(9, SIGFPE, "coprocessor segment overrun", coprocessor_segment_overrun)
DO_ERROR(10, SIGSEGV, "invalid TSS", invalid_TSS)
DO_ERROR(11, SIGBUS, "segment not present", segment_not_present)
#ifdef CONFIG_X86_32
DO_ERROR(12, SIGBUS, "stack segment", stack_segment)
#endif
DO_ERROR_INFO(17, SIGBUS, "alignment check", alignment_check, BUS_ADRALN, 0)


/*
 * Note that we play around with the 'TS' bit in an attempt to get
 * the correct behaviour even in the presence of the asynchronous
 * IRQ13 behaviour
 */
void math_error(struct pt_regs *regs, int error_code, int trapnr)
{
	printk(DEBUG_PREFIX"数学处理器异常,程序无法运行");
	fak_arch_x86_dump_register(regs);	
	fak_thread_want_die();
}

asmregparm void _do_coprocessor_error(struct pt_regs *regs, long error_code)
{
#ifdef CONFIG_X86_32
	ignore_fpu_irq = 1;
#endif

	math_error(regs, error_code, 16);
}

asmregparm void
_do_simd_coprocessor_error(struct pt_regs *regs, long error_code)
{
	math_error(regs, error_code, 19);
}
void _do_spurious_interrupt_bug(struct pt_regs *regs, long error_code)
{
#if 0
	conditional_sti(regs);

	/* No need to warn about this any longer. */
	printk(HAL_INFO "Ignoring P6 Local APIC Spurious Interrupt Bug...\n");
#endif
}
asmregparm void _do_general_protection(struct pt_regs *regs, long error_code)
{
	printk(DEBUG_PREFIX"一般保护错误,程序无法运行,错误代码 %h.", error_code);
	fak_arch_x86_dump_register(regs);	
	fak_thread_want_die();
}

void __init arch_trap_init()
{
	int i;

	//set_intr_gate_ist(1, &debug, DEBUG_STACK);
	/* int3 can be called from all */
	//set_system_intr_gate_ist(3, &int3, DEBUG_STACK);
	set_intr_gate(14, &page_fault);

	set_intr_gate(0, &divide_error);
	set_intr_gate_ist(2, &nmi, NMI_STACK);
	/* int4 can be called from all */
	set_system_intr_gate(4, &overflow);
	set_intr_gate(5, &bounds);
	set_intr_gate(6, &invalid_op);
	set_intr_gate(7, &device_not_available);
#ifdef CONFIG_X86_32
	//set_task_gate(8, GDT_ENTRY_DOUBLEFAULT_TSS);
	extern void double_fault();
	set_intr_gate(8,double_fault);
#else
	set_intr_gate_ist(8, &double_fault, DOUBLEFAULT_STACK);
#endif
	set_intr_gate(9, &coprocessor_segment_overrun);
	set_intr_gate(10, &invalid_TSS);
	set_intr_gate(11, &segment_not_present);
	set_intr_gate_ist(12, &stack_segment, STACKFAULT_STACK);
	set_intr_gate(13, &general_protection);
	set_intr_gate(15, &spurious_interrupt_bug);
	set_intr_gate(16, &coprocessor_error);
	set_intr_gate(17, &alignment_check);
#if 0
#ifdef CONFIG_X86_MCE
	set_intr_gate_ist(18, &machine_check, MCE_STACK);
#endif
#endif
	set_intr_gate(19, &simd_coprocessor_error);
	/* Reserve all the builtin and the syscall vector: */
	for (i = 0; i < FIRST_EXTERNAL_VECTOR; i++)
		set_bit(i, used_vectors);
	
	/*
		System call entry
	*/
	extern unsigned long asm_system_call();
	set_system_trap_gate(0x87, &asm_system_call);

	/* 
		Cpu init is placed to arch_hal_init 
	*/
}
