
#include <irq.h>
#include <irqdesc.h>
#include <asm/ptrace.h>

static inline int check_stack_overflow(void) { return 0; }
static inline void print_stack_overflow(void) { }

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
