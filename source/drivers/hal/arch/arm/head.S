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

	.arm

	__HEAD
#define ARCH_BOOT_STACK_SIZE 0x8000
arch_boot_stack:
	.fill ARCH_BOOT_STACK_SIZE

ENTRY(stext)
	@ ensure svc mode and all interrupts masked
	safe_svcmode_maskall r9

	mov sp,#CONFIG_HAL_KERNEL_BASE								@ set stack;
	orr sp,sp,#ARCH_BOOT_STACK_SIZE

	/*Goto C code*/
	b __arm_main0

ENDPROC(stext)

/*
 r0 = table base
 */
ENTRY(arch_enable_mmu)
	@ ensure svc mode and all interrupts masked
	safe_svcmode_maskall r9

	mrc	p15, 0, r9, c0, c0			@ get processor id
	bl	__lookup_processor_type		@ r5=procinfo r9=cpuid
	movs	r10, r5					@ invalid processor (r5=0)?

	/*
		r4 is the page table base for enable_mmu
	*/
	mov	r4, r0

	/*
		跳板代码,先经过PROCINFO_INITFUNC初始化CPU，获得C1寄存器，然后就到ENABLE_MMU，然后到SWITCH_DATA。
	*/
	ldr	r13, =__mmap_switched		@ address to jump to after
									@ mmu has been enabled
	adr	lr, BSYM(1f)				@ return (PIC) address
	mov	r8, r4				@ set TTBR1 to swapper_pg_dir
	ARM(	add	pc, r10, #PROCINFO_INITFUNC	)
	THUMB(	add	r12, r10, #PROCINFO_INITFUNC	)
	THUMB(	mov	pc, r12				)
1:	b	__enable_mmu
ENDPROC(arch_enable_mmu)

/*
 * Setup common bits before finally enabling the MMU.  Essentially
 * this is just loading the page table pointer and domain access
 * registers.
 *
 *  r0  = cp#15 control register
 *  r1  = machine ID
 *  r2  = atags or dtb pointer
 *  r4  = page table pointer
 *  r9  = processor ID
 *  r13 = *virtual* address to jump to upon completion
 */
__enable_mmu:
#if defined(CONFIG_ALIGNMENT_TRAP) && __LINUX_ARM_ARCH__ < 6
	orr	r0, r0, #CR_A
#else
	bic	r0, r0, #CR_A
#endif
#ifdef CONFIG_CPU_DCACHE_DISABLE
	bic	r0, r0, #CR_C
#endif
#ifdef CONFIG_CPU_BPREDICT_DISABLE
	bic	r0, r0, #CR_Z
#endif
#ifdef CONFIG_CPU_ICACHE_DISABLE
	bic	r0, r0, #CR_I
#endif
#ifdef CONFIG_ARM_LPAE
	mov	r5, #0
	mcrr	p15, 0, r4, r5, c2		@ load TTBR0
#else
	mov	r5, #(domain_val(DOMAIN_USER, DOMAIN_MANAGER) | \
	domain_val(DOMAIN_KERNEL, DOMAIN_MANAGER) | \
	domain_val(DOMAIN_TABLE, DOMAIN_MANAGER) | \
	domain_val(DOMAIN_IO, DOMAIN_CLIENT))
	mcr	p15, 0, r5, c3, c0, 0		@ load domain access register
	mcr	p15, 0, r4, c2, c0, 0		@ load page table pointer
#endif
	b	__turn_mmu_on
ENDPROC(__enable_mmu)


/*
 * Enable the MMU.  This completely changes the structure of the visible
 * memory space. 
 *
 *  r0  = cp#15 control register
 *  r1  = machine ID
 *  r2  = atags or dtb pointer
 *  r9  = processor ID
 *  r13 = *virtual* address to jump to upon completion
 *
 * other registers depend on the function called upon completion
 */
.align	5
ENTRY(__turn_mmu_on)
	mov	r0, r0
	instr_sync
	mcr	p15, 0, r0, c1, c0, 0		@ write control reg
	mrc	p15, 0, r3, c0, c0, 0		@ read id reg
	instr_sync
	mov	r3, r3
	mov	r3, r13
	mov	pc, r3
__turn_mmu_on_end:
ENDPROC(__turn_mmu_on)


#include "head-common.S"

