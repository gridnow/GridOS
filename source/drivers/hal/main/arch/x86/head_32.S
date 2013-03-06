/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   X86 32 入口
*/
	.text

#include <linkage.h>

#define startup_sp_size 16384
#define __HEAD		.section	".head.text","ax"

	__HEAD
ENTRY(start)
ENTRY(_stext)
	call arch_early_init
	
	/*
		Switch to my stack 
	*/
	lea startup_sp, %eax
	addl $startup_sp_size, %eax
	mov %eax, %esp

	/*
		Switch to high address
		跳板程序将执行hal_main
	*/
	jmp tramp_to_high
	
/**
	@brief jump to 32bit high address to run 
	
	The loader placed as to low physical.
*/
x86_jump_to_32bit_high:	
	.globl x86_jump_to_32bit_high	
	movl 12(%esp),%ecx							/*get new ds*/
	mov %cx,%ds
	mov %cx,%es
	mov %cx,%ss
	xor %cx,%cx
	mov %cx,%fs
	mov %cx,%gs
	
	movl 0(%esp),%eax;							/*get ip*/
	movl 4(%esp),%ecx;							/*cs,base is 0*/
	addl 8(%esp),%eax;							/*new eip = old ip + base*/
	
	addl $4,%esp								/*delete the old ip,to keep stack same as caller*/
	pushl %ecx
	pushl %eax

	retf
/***************************************************
default stack for startup thread
****************************************************/
	.data
startup_sp:
	.fill startup_sp_size	
