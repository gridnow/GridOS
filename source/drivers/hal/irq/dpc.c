/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Sihai.Yao
*   HAL 延迟过程调用DPC中断模块
*/

#include <irq.h>
#include <cache.h>
#include <hardirq.h>
#include <preempt.h>
#include <irqflags.h>
#include <smp.h>
enum
{
	HI_DPC_IRQ  = 0,
	
	
	NR_DPC_IRQS = BITS_PER_LONG
};

struct dpc_irq_action
{
	void	(*action)(struct dpc_irq_action *);
};

static struct dpc_irq_action dpc_irq_vec[NR_DPC_IRQS] __cacheline_aligned_in_smp;

/* Totally we have MAX DPC threads to run on all CPUs */
#define MAX_DPC_THREADS 1
static void *dpc_threads[MAX_DPC_THREADS];

//TODO:多处理器上每个处理器由自己的pending
static unsigned long dpc_irq_pending;
static unsigned long local_dpc_irq_pending()
{	
	return dpc_irq_pending;
}

static void set_dpc_irq_pending(unsigned long what)
{
	dpc_irq_pending = what;
}

static void or_dpc_irq_pending(unsigned long bits)
{
	dpc_irq_pending |= bits;
}

static void __local_bh_enable(unsigned int cnt)
{
	hal_preempt_count_sub(cnt);
}

static inline void __local_bh_disable(unsigned long ip, unsigned int cnt)
{
	hal_preempt_count_add(cnt);
	barrier();
}

static void wakeup_dpc_irqd()
{

}

#define MAX_SOFTIRQ_RESTART 10
asmlinkage void __do_dpc_irq(void)
{
	struct dpc_irq_action *h;
	unsigned long pending;
	int max_restart = MAX_SOFTIRQ_RESTART;
	int cpu;

	/* Get mask */
	local_irq_disable();
	pending = local_dpc_irq_pending();
	__local_bh_disable((unsigned long)__builtin_return_address(0),
				SOFTIRQ_OFFSET);
	cpu = smp_processor_id();
restart:
	/* Reset the pending bit mask before enabling irqs */
	set_dpc_irq_pending(0);
	local_irq_enable();

	/* Handle it */
	h = dpc_irq_vec;
	do {
		if (pending & 1) {
			unsigned int vec_nr = h - dpc_irq_vec;
			int prev_count = hal_preempt_count();
			
			h->action(h);

			if (unlikely(prev_count != hal_preempt_count())) {
				hal_preempt_count() = prev_count;
			}
		}
		h++;
		pending >>= 1;
	} while (pending);

	local_irq_disable();
	pending = local_dpc_irq_pending();
	if (pending && --max_restart)
		goto restart;
	if (pending)
		wakeup_dpc_irqd();

	__local_bh_enable(SOFTIRQ_OFFSET);
}

static void dpc_irq_thread()
{
	while(1)
	{
		local_irq_disable();
		if (local_dpc_irq_pending())
			__do_dpc_irq();
		local_irq_enable();
	}	
}

/*
 * Enter an interrupt context.
 */
void irq_enter(void)
{
	hal_preempt_count_add(HARDIRQ_OFFSET);
}

/*
 * Exit an interrupt context. Process dpc_irqs if needed and possible:
 */
void irq_exit(void)
{
	hal_preempt_count_sub(IRQ_EXIT_OFFSET);

	/* 如果有中断要处理，唤醒处理线程 */
 	if (local_dpc_irq_pending())
		wakeup_dpc_irqd();
	
	hal_preempt_enable_no_resched();
}

void __raise_dpc_irq_irqoff(unsigned int nr)
{
	or_dpc_irq_pending(1UL << nr);
}

inline void raise_dpc_irq_irqoff(unsigned int nr)
{
	__raise_dpc_irq_irqoff(nr);

	if (!in_interrupt())
		wakeup_dpc_irqd();
}

void raise_dpc_irq(unsigned int nr)
{
	unsigned long flags;

	local_irq_save(flags);
	raise_dpc_irq_irqoff(nr);
	local_irq_restore(flags);
}

void open_dpc_irq(int nr, void (*action)(struct dpc_irq_action *))
{
	dpc_irq_vec[nr].action = action;
}

void __init dpc_irq_init(void)
{

}