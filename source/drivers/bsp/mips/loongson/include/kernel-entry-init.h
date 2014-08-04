
#ifndef __ASM_MACH_LOONGSON_HAL_ENTRY_H
#define __ASM_MACH_LOONGSON_HAL_ENTRY_H

	.macro	kernel_entry_setup
#ifdef CONFIG_NUMA
	.set	push
	.set	mips64
	/* Set LPA on LOONGSON3 config3 */
	mfc0	t0, $6, 4
	or	t0, (0x1 << 7)
	mtc0	t0, $6, 4
	/* Set ELPA on LOONGSON3 pagegrain */
	li	t0, (0x1 << 29) 
	mtc0	t0, $5, 1 
	_ehb
	.set	pop
#endif
	.endm
	
/*
 * Do SMP slave processor setup.
 */
	.macro	smp_slave_setup
#ifdef CONFIG_NUMA
	.set	push
	.set	mips64
	/* Set LPA on LOONGSON3 config3 */
	mfc0	t0, $6, 4
	or	t0, (0x1 << 7)
	mtc0	t0, $6, 4
	/* Set ELPA on LOONGSON3 pagegrain */
	li	t0, (0x1 << 29) 
	mtc0	t0, $5, 1 
	_ehb
	.set	pop
#endif
	.endm
	
#endif
