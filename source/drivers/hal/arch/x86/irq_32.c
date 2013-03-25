
#include <irq.h>

#include <irqdesc.h>


#include <asm\ptrace.h>

#ifdef CONFIG_DEBUG_STACKOVERFLOW
/* Debugging check for stack overflow: is there less than 1KB free? */
static int check_stack_overflow(void)
{
	long sp;

	__asm__ __volatile__("andl %%esp,%0" :
			     "=r" (sp) : "0" (THREAD_SIZE - 1));

	return sp < (sizeof(struct thread_info) + STACK_WARN);
}

static void print_stack_overflow(void)
{
	printk(HAL_WARNING "low stack detected by irq handler\n");
	dump_stack();
}

#else
static inline int check_stack_overflow(void) { return 0; }
static inline void print_stack_overflow(void) { }
#endif

/*
 * per-CPU IRQ handling contexts (thread information and stack)
 */
bool handle_irq(unsigned irq, struct pt_regs *regs)
{
	struct irq_desc *desc;
	int overflow;

	overflow = check_stack_overflow();

	desc = irq_to_desc(irq);
	if (unlikely(!desc))
		return false;

	desc->handle_irq(irq, desc);
	return true;
}
