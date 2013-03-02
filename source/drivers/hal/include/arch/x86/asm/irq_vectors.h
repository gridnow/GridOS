#ifndef _ASM_X86_IRQ_VECTORS_H
#define _ASM_X86_IRQ_VECTORS_H

/*
* IDT vectors usable for external interrupt sources start at 0x20.
* (0x78 is the syscall vector, 0x30-0x3f are for ISA)
*/
#define FIRST_EXTERNAL_VECTOR		0x20

/*
 * Vectors 0x30-0x3f are used for ISA interrupts.
 *   round up to the next 16-vector boundary
 */
#define IRQ0_VECTOR			((FIRST_EXTERNAL_VECTOR + 16) & ~15)

#define IRQ1_VECTOR			(IRQ0_VECTOR +  1)
#define IRQ2_VECTOR			(IRQ0_VECTOR +  2)
#define IRQ3_VECTOR			(IRQ0_VECTOR +  3)
#define IRQ4_VECTOR			(IRQ0_VECTOR +  4)
#define IRQ5_VECTOR			(IRQ0_VECTOR +  5)
#define IRQ6_VECTOR			(IRQ0_VECTOR +  6)
#define IRQ7_VECTOR			(IRQ0_VECTOR +  7)
#define IRQ8_VECTOR			(IRQ0_VECTOR +  8)
#define IRQ9_VECTOR			(IRQ0_VECTOR +  9)
#define IRQ10_VECTOR			(IRQ0_VECTOR + 10)
#define IRQ11_VECTOR			(IRQ0_VECTOR + 11)
#define IRQ12_VECTOR			(IRQ0_VECTOR + 12)
#define IRQ13_VECTOR			(IRQ0_VECTOR + 13)
#define IRQ14_VECTOR			(IRQ0_VECTOR + 14)
#define IRQ15_VECTOR			(IRQ0_VECTOR + 15)

#define NR_VECTORS			 256

/*
 * Special IRQ vectors used by the SMP architecture, 0xf0-0xff
 *
 *  some of the following vectors are 'rare', they are merged
 *  into a single vector (CALL_FUNCTION_VECTOR) to save vector space.
 *  TLB, reschedule and local APIC vectors are performance-critical.
 */

#define SPURIOUS_APIC_VECTOR		0xff

/* apic id */
#define ERROR_APIC_VECTOR		0xfe
#define RESCHEDULE_VECTOR		0xfd
#define CALL_FUNCTION_VECTOR		0xfc
#define CALL_FUNCTION_SINGLE_VECTOR	0xfb
#define THERMAL_APIC_VECTOR		0xfa
#define THRESHOLD_APIC_VECTOR		0xf9
#define REBOOT_VECTOR			0xf8

/*
 * Generic system vector for platform specific use
 */
#define X86_PLATFORM_IPI_VECTOR		0xf7

/*
 * IRQ work vector:
 */
#define IRQ_WORK_VECTOR			0xf6

#define UV_BAU_MESSAGE			0xf5

/* Xen vector callback to receive events in a HVM domain */
#define XEN_HVM_EVTCHN_CALLBACK		0xf3

/*
 * Local APIC timer IRQ vector is on a different priority level,
 * to work around the 'lost local interrupt if more than 2 IRQ
 * sources per level' errata.
 */
#define LOCAL_TIMER_VECTOR		0xef

#define INVALIDATE_TLB_VECTOR_START 0xee

#define NR_IRQS_LEGACY			  16

#endif