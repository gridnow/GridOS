/*
	正对X86-32的杂项处理，包括异常处理等
	wuxin
*/

#include <linkage.h>
#include <asm/irq_vectors.h>
#include <asm/segment.h>

/*
	一些杂项的处理
*/
	.text
	
/********************************************************************
	异常处理
********************************************************************/

/*
 * Some functions should be protected against kprobes
 */
	//.pushsection .kprobes.text, "ax"
	.text
/* 合成PT_REG */
#define PT_EBX 0 /* offsetof(struct pt_regs, bx)	# */
#define PT_ECX 4 /* offsetof(struct pt_regs, cx)	# */
#define PT_EDX 8 /* offsetof(struct pt_regs, dx)	# */
#define PT_ESI 12 /* offsetof(struct pt_regs, si)	# */
#define PT_EDI 16 /* offsetof(struct pt_regs, di)	# */
#define PT_EBP 20 /* offsetof(struct pt_regs, bp)	# */
#define PT_EAX 24 /* offsetof(struct pt_regs, ax)	# */
#define PT_DS 28 /* offsetof(struct pt_regs, ds)	# */
#define PT_ES 32 /* offsetof(struct pt_regs, es)	# */
#define PT_FS 36 /* offsetof(struct pt_regs, fs)	# */
#define PT_GS 40 /* offsetof(struct pt_regs, gs)	# */
#define PT_ORIG_EAX 44 /* offsetof(struct pt_regs, orig_ax)	# */
#define PT_EIP 48 /* offsetof(struct pt_regs, ip)	# */
#define PT_CS 52 /* offsetof(struct pt_regs, cs)	# */
#define PT_EFLAGS 56 /* offsetof(struct pt_regs, flags)	# */
#define PT_OLDESP 60 /* offsetof(struct pt_regs, sp)	# */
#define PT_OLDSS 64 /* offsetof(struct pt_regs, ss)	# */

ENTRY(page_fault)
	pushl $do_page_fault
	ALIGN
error_code:
	pushl %fs	
	pushl %es	
	pushl %ds	
	pushl %eax	
	pushl %ebp	
	pushl %edi	
	pushl %esi	
	pushl %edx	
	pushl %ecx	
	pushl %ebx	
	cld
	
	/* Change to kernel FS cpu */
	movl $(__KERNEL_PERCPU), %edx
	movl %edx, %fs
	
	movl PT_GS(%esp), %edi		# get the function address
	movl PT_ORIG_EAX(%esp), %edx	# get the error code
	movl %esp,%eax			# pt_regs pointer
	call *%edi
	
	popl %ebx	
	popl %ecx	
	popl %edx	
	popl %esi
	popl %edi	
	popl %ebp
	popl %eax	
	popl %ds
	popl %es
	popl %fs		
		
	addl $8, %esp			# GS 既没有保存也没有恢复 + 错误码 和入口地址
	/* Add code here 支持返回到16位堆栈恢复ESP高16位的问题, VM86 */

	iret
END(page_fault)

ENTRY(double_fault)
	pushl $_do_double_fault
	jmp error_code
END(double_fault)


ENTRY(divide_error)
	pushl $0			# no error code
	pushl $_do_divide_error
	jmp error_code
END(divide_error)

/*
 * NMI is doubly nasty. It can happen _while_ we're handling
 * a debug fault, and the debug fault hasn't yet been able to
 * clear up the stack. So we first check whether we got  an
 * NMI on the sysenter entry path, but after that we need to
 * check whether we got an NMI on the debug path where the debug
 * fault happened on the sysenter path.
 */
ENTRY(nmi)
	iret
END(nmi)


ENTRY(overflow)
	pushl $0
	pushl $_do_overflow
	jmp error_code
END(overflow)

ENTRY(bounds)
	pushl $0
	pushl $_do_bounds
	jmp error_code
END(bounds)

ENTRY(invalid_op)
	pushl $0
	pushl $_do_invalid_op
	jmp error_code
END(invalid_op)



ENTRY(coprocessor_segment_overrun)
	pushl  $0
	pushl  $_do_coprocessor_segment_overrun
	jmp error_code
END(coprocessor_segment_overrun)

ENTRY(invalid_TSS)
	pushl $_do_invalid_TSS
	jmp error_code
END(invalid_TSS)

ENTRY(segment_not_present)
	pushl $_do_segment_not_present
	jmp error_code
END(segment_not_present)

ENTRY(stack_segment)
	pushl $_do_stack_segment
	jmp error_code
END(stack_segment)

ENTRY(alignment_check)	
	pushl $_do_alignment_check
	jmp error_code
END(alignment_check)

ENTRY(spurious_interrupt_bug)
	pushl $0
	pushl $_do_spurious_interrupt_bug
	jmp error_code
END(spurious_interrupt_bug)

ENTRY(coprocessor_error)
	pushl $0
	pushl $_do_coprocessor_error
	jmp error_code
END(coprocessor_error)

ENTRY(simd_coprocessor_error)
	pushl $0
#ifdef CONFIG_X86_INVD_BUG
	/* AMD 486 bug: invd from userspace calls exception 19 instead of #GP */
661:	pushl $_do_general_protection
662:
.section .altinstructions,"a"
	.balign 4
	.long 661b
	.long 663f
	.word X86_FEATURE_XMM
	.byte 662b-661b
	.byte 664f-663f
.previous
.section .altinstr_replacement,"ax"
663:	pushl $_do_simd_coprocessor_error
664:
.previous
#else
	pushl $_do_simd_coprocessor_error
#endif
	jmp error_code
END(simd_coprocessor_error)

ENTRY(device_not_available)
	pushl $-1			# mark this as an int
	pushl $_do_device_not_available
	jmp error_code
END(device_not_available)


ENTRY(general_protection)
	pushl $_do_general_protection
	jmp error_code
END(general_protection)

#if 0
#ifdef CONFIG_X86_MCE
ENTRY(machine_check)
	pushl $0
	pushl _machine_check_vector
	jmp error_code
END(machine_check)
#endif
#endif


/***********************************************
HARDWARE IRQ
************************************************/
/*
 * Build the entry stubs and pointer table with some assembler magic.
 * We pack 7 stubs into a single 32-byte chunk, which will fit in a
 * single cache line on all modern x86 implementations.
 */
.section .init.rodata,"a"
ENTRY(interrupt)
.text
	.p2align 5
	.p2align CONFIG_X86_L1_CACHE_SHIFT
ENTRY(irq_entries_start)
//	RING0_INT_FRAME
vector=FIRST_EXTERNAL_VECTOR
.rept (NR_VECTORS-FIRST_EXTERNAL_VECTOR+6)/7
	.balign 32
  .rept	7
    .if vector < NR_VECTORS
      .if vector <> FIRST_EXTERNAL_VECTOR
//	CFI_ADJUST_CFA_OFFSET -4
      .endif
//1:	pushl_cfi $(~vector+0x80)	/* Note: always in signed byte range */
1:	pushl $(~vector+0x80)
      .if ((vector-FIRST_EXTERNAL_VECTOR)%7) <> 6
	jmp 2f
      .endif
      .previous
	.long 1b
      .text
vector=vector+1
    .endif
  .endr
2:	jmp common_interrupt
.endr
END(irq_entries_start)

.previous
END(interrupt)
.previous

/*
 * the CPU automatically disables interrupts when executing an IRQ vector,
 * so IRQ-flags tracing has to follow that:
 */
	.p2align CONFIG_X86_L1_CACHE_SHIFT
common_interrupt:
	addl $-0x80,(%esp)	/* Adjust vector into the [-256,-1] range */
	
	pushl %gs	
	pushl %fs	
	pushl %es	
	pushl %ds	
	pushl %eax	
	pushl %ebp	
	pushl %edi	
	pushl %esi	
	pushl %edx	
	pushl %ecx	
	pushl %ebx	
	cld
	
	/* Change to kernel FS cpu */
	movl $(__KERNEL_PERCPU), %edx
	movl %edx, %fs
	
	movl %esp,%eax
	call do_IRQ
	
	popl %ebx	
	popl %ecx	
	popl %edx	
	popl %esi
	popl %edi	
	popl %ebp
	popl %eax
	popl %ds
	popl %es
	popl %fs
	popl %gs
	
	addl $4,%esp		# +1 vector;
	iret	
ENDPROC(common_interrupt)

/***********************************************
系统请求入口
************************************************/
ENTRY(asm_system_call)
	/*
		x86 32位是这样的 
		EAX=ReqPackage
	*/
	PUSH %fs
	/* Change to kernel FS cpu */
	movl $(__KERNEL_PERCPU), %edx
	movl %edx, %fs
	sti

	call arch_system_call	
	POP %fs 
	iret
END(asm_system_call)