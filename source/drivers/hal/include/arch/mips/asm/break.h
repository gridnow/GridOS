#ifndef __ASM_BREAK_H
#define __ASM_BREAK_H

#define BRK_USERBP	0	/* User bp (used by debuggers) */
#define BRK_KERNELBP	1	/* Break in the kernel */
#define BRK_ABORT	2	/* Sometimes used by abort(3) to SIGIOT */
#define BRK_BD_TAKEN	3	/* For bd slot emulation - not implemented */
#define BRK_BD_NOTTAKEN	4	/* For bd slot emulation - not implemented */
#define BRK_SSTEPBP	5	/* User bp (used by debuggers) */
#define BRK_OVERFLOW	6	/* Overflow check */
#define BRK_DIVZERO	7	/* Divide by zero check */
#define BRK_RANGE	8	/* Range error check */
#define BRK_STACKOVERFLOW 9	/* For Ada stackchecking */
#define BRK_NORLD	10	/* No rld found - not used by Linux/MIPS */
#define _BRK_THREADBP	11	/* For threads, user bp (used by debuggers) */
#define BRK_BUG		512	/* Used by BUG() */
#define BRK_KDB		513	/* Used in KDB_ENTER() */
#define BRK_MEMU	514	/* Used by FPU emulator */
#define BRK_KPROBE_BP	515	/* Kprobe break */
#define BRK_KPROBE_SSTEPBP 516	/* Kprobe single step software implementation */
#define BRK_MULOVF	1023	/* Multiply overflow */

#endif /* __ASM_BREAK_H */
