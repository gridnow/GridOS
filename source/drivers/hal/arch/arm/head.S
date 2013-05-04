/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin
 *   ARM 平台初始化
 *
 */
#include <linkage.h>
#include <asm/assembler.h>

#define __HEAD		.section	".head.text","ax"
	.arm

	__HEAD
.fill 0x20000

ENTRY(stext)
	@ ensure svc mode and all interrupts masked
	safe_svcmode_maskall r9

	mov sp,#CONFIG_HAL_KERNEL_BASE								@ set stack;
	orr sp,sp,#0x20000

	/*Goto C code*/
	b setup_arch

ENDPROC(stext)