
#include <asm/irq_vectors.h>
#include <asm/desc.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/i8259.h>
#include <asm/desc.h>
#include <asm/hw_irq.h>
#include <asm/x86_init.h>

#include <irq.h>
#include <debug.h>
extern void (*__initconst interrupt[NR_VECTORS-FIRST_EXTERNAL_VECTOR])(void);		//org from hw_irq.h

/*
 * IRQ2 is cascade interrupt to second interrupt controller
 */
static struct irqaction irq2 = {
	.handler = no_action,
	.name = "cascade",
	.flags = IRQF_NO_THREAD,
};

vector_irq_t vector_irq = {
	[0 ... NR_VECTORS - 1] = -1,
};

#ifdef CONFIG_SMP
static void reschedule_interrupt()
{
	TODO("");
}

static void invalidate_interrupt()
{
	TODO("");
}

static void call_function_interrupt()
{
	TODO("");
}

static void call_function_single_interrupt()
{
	TODO("");
}

static void reboot_interrupt()
{
	TODO("");
}

#endif

#ifdef CONFIG_X86_LOCAL_APIC
static void apic_timer_interrupt()
{
	TODO("");
}

static void spurious_interrupt()
{
	TODO("");
}

static void x86_platform_ipi()
{
	TODO("");
}

static void error_interrupt()
{
	TODO("");
}

#endif

static void __init smp_intr_init(void)
{
#ifdef CONFIG_SMP
#if defined(CONFIG_X86_64) || defined(CONFIG_X86_LOCAL_APIC)
	/*
	 * The reschedule interrupt is a CPU-to-CPU reschedule-helper
	 * IPI, driven by wakeup.
	 */
	alloc_intr_gate(RESCHEDULE_VECTOR, reschedule_interrupt);

	/* IPIs for invalidation */
	alloc_intr_gate(INVALIDATE_TLB_VECTOR_START, invalidate_interrupt);

	/* IPI for generic function call */
	alloc_intr_gate(CALL_FUNCTION_VECTOR, call_function_interrupt);

	/* IPI for generic single function call */
	alloc_intr_gate(CALL_FUNCTION_SINGLE_VECTOR,
			call_function_single_interrupt);

	/* IPI used for rebooting/stopping */
	alloc_intr_gate(REBOOT_VECTOR, reboot_interrupt);
#endif
#endif /* CONFIG_SMP */
}

static void __init apic_intr_init(void)
{
	smp_intr_init();

#ifdef CONFIG_X86_THERMAL_VECTOR
	alloc_intr_gate(THERMAL_APIC_VECTOR, thermal_interrupt);
#endif
#ifdef CONFIG_X86_MCE_THRESHOLD
	alloc_intr_gate(THRESHOLD_APIC_VECTOR, threshold_interrupt);
#endif

#if defined(CONFIG_X86_64) || defined(CONFIG_X86_LOCAL_APIC)
	/* self generated IPI for local APIC timer */
	alloc_intr_gate(LOCAL_TIMER_VECTOR, apic_timer_interrupt);

	/* IPI for X86 platform specific use */
	alloc_intr_gate(X86_PLATFORM_IPI_VECTOR, x86_platform_ipi);

	/* IPI vectors for APIC spurious and error interrupts */
	alloc_intr_gate(SPURIOUS_APIC_VECTOR, spurious_interrupt);
	alloc_intr_gate(ERROR_APIC_VECTOR, error_interrupt);

	/* IRQ work interrupts: */
# ifdef CONFIG_IRQ_WORK
	alloc_intr_gate(IRQ_WORK_VECTOR, irq_work_interrupt);
# endif

#endif
}

void __init init_ISA_irqs(void)
{
	struct irq_chip *chip = legacy_pic->chip;
	const char *name = chip->name;
	int i;

	/* init_bsp_APIC(); is called in smp setup */

	/* Init legacy pic */
	legacy_pic->init(0);

	for (i = 0; i < legacy_pic->nr_legacy_irqs; i++)
		irq_set_chip_and_handler_name(i, chip, handle_level_irq, name);
}

void native_init_IRQ()
{
	int i;
	/* Execute any quirks before the call gates are initialised: */
	init_ISA_irqs();

	apic_intr_init();

	/*
	* Cover the whole vector space, no vector can escape
	* us. (some of these will be overridden and become
	* 'special' SMP interrupts)
	*/
	for (i = FIRST_EXTERNAL_VECTOR; i < NR_VECTORS; i++) {
		/* IA32_SYSCALL_VECTOR could be used in trap_init already. */
		if (!test_bit(i, used_vectors))			
			set_intr_gate(i, interrupt[i-FIRST_EXTERNAL_VECTOR]);		
	} 

	setup_irq(2, &irq2);
}

void __init arch_init_irq(void)
{
	int i;

	/*
	 * On cpu 0, Assign IRQ0_VECTOR..IRQ15_VECTOR's to IRQ 0..15.
	 * If these IRQ's are handled by legacy interrupt-controllers like PIC,
	 * then this configuration will likely be static after the boot. If
	 * these IRQ's are handled by more mordern controllers like IO-APIC,
	 * then this vector space can be freed and re-used dynamically as the
	 * irq's migrate etc.
	 */
	for (i = 0; i < legacy_pic->nr_legacy_irqs; i++)
		vector_irq[IRQ0_VECTOR + i] = i;

	/* Actually this is for Xen/etc platform */
	x86_init.irqs.intr_init();	
}