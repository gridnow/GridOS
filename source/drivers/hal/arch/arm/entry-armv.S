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
#include <asm/cp15.h>
#include <asm/asm-offsets.h>

	.data

.globl	cr_alignment
.globl	cr_no_alignment
cr_alignment:
	.space	4
cr_no_alignment:
	.space	4

#ifdef CONFIG_MULTI_IRQ_HANDLER
	.globl	handle_arch_irq
handle_arch_irq:
	.space	4
#endif
