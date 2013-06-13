/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin, LuYan
*   HAL的软件定时器
*/
#include <types.h>
#include <list.h>
#include <errno.h>
#include <ddk/compatible.h>
#include <ddk/slab.h>
#include <ddk/debug.h>

#include <bits.h>

#include <cache.h>
#include <spinlock.h>
#include <percpu.h>
#include <tick.h>
#include <preempt.h>
#include <irq.h>
#include <smp.h>
#include <timer.h>
#include <topology.h>

#define CONFIG_BASE_SMALL 0

/***************************************************/
#define TIMER_NOT_PINNED	0
#define TIMER_PINNED		1

#define TIMER_DEFERRABLE		0x1LU
#define TIMER_IRQSAFE			0x2LU

#define TIMER_FLAG_MASK			0x3LU

struct tvec_base;
struct timer_list {
	/*
	 * All fields that change during normal runtime grouped to the
	 * same cacheline
	 */
	struct list_head entry;
	unsigned long expires;
	struct tvec_base *base;
	
	void (*function)(unsigned long);
	unsigned long data;
	
	int slack;
};

static inline int timer_pending(const struct timer_list * timer)
{
	return timer->entry.next != NULL;
}
/***************************************************/

#define TVN_BITS (CONFIG_BASE_SMALL ? 4 : 6)
#define TVR_BITS (CONFIG_BASE_SMALL ? 6 : 8)
#define TVN_SIZE (1 << TVN_BITS)
#define TVR_SIZE (1 << TVR_BITS)
#define TVN_MASK (TVN_SIZE - 1)
#define TVR_MASK (TVR_SIZE - 1)
#define MAX_TVAL ((unsigned long)((1ULL << (TVR_BITS + 4*TVN_BITS)) - 1))


struct tvec {
	struct list_head vec[TVN_SIZE];
};

struct tvec_root {
	struct list_head vec[TVR_SIZE];
};

struct tvec_base {
	spinlock_t lock;
	struct timer_list *running_timer;
	unsigned long timer_jiffies;
	unsigned long next_timer;
	unsigned long active_timers;
	struct tvec_root tv1;
	struct tvec tv2;
	struct tvec tv3;
	struct tvec tv4;
	struct tvec tv5;
} ____cacheline_aligned;

u64 jiffies_64 __cacheline_aligned_in_smp = INITIAL_JIFFIES;

struct tvec_base boot_tvec_bases;
static DEFINE_PER_CPU(struct tvec_base *, tvec_bases) = &boot_tvec_bases;

/*
 * Note that all tvec_bases are 2 byte aligned and lower bit of
 * base in timer_list is guaranteed to be zero. Use the LSB for
 * the new flag to indicate whether the timer is deferrable
 */

/* Functions below help us manage 'deferrable' flag */
static inline unsigned int tbase_get_deferrable(struct tvec_base *base)
{
	return ((unsigned int)(unsigned long)base & TIMER_DEFERRABLE);
}

static inline unsigned int tbase_get_irqsafe(struct tvec_base *base)
{
	return ((unsigned int)(unsigned long)base & TIMER_IRQSAFE);
}

static inline struct tvec_base *tbase_get_base(struct tvec_base *base)
{
	return ((struct tvec_base *)((unsigned long)base & ~TIMER_FLAG_MASK));
}

static inline void timer_set_base(struct timer_list *timer, struct tvec_base *new_base)
{
	unsigned long flags = (unsigned long)timer->base & TIMER_FLAG_MASK;
	
	timer->base = (struct tvec_base *)((unsigned long)(new_base) | flags);
}

static int __cpuinit init_timers_cpu(int cpu)
{
	int j;
	struct tvec_base *base;
	static char __cpuinitdata tvec_base_done[NR_CPUS];
	
	if (!tvec_base_done[cpu]) {
		static char boot_done;
		
		if (boot_done) {
			/*
			 * The APs use this path later in boot
			 */
			base = kmalloc_node(sizeof(*base),
							  GFP_KERNEL | __GFP_ZERO,
							  cpu_to_node(cpu));
			if (!base)
				return -ENOMEM;
			
			/* Make sure that tvec_base is 2 byte aligned */
			if (tbase_get_deferrable(base)) {
				WARN_ON(1);
				kfree(base);
				return -ENOMEM;
			}
			per_cpu(tvec_bases, cpu) = base;
		} else {
			/*
			 * This is for the boot CPU - we use compile-time
			 * static initialisation because per-cpu memory isn't
			 * ready yet and because the memory allocators are not
			 * initialised either.
			 */
			boot_done = 1;
			base = &boot_tvec_bases;
			per_cpu(tvec_bases, cpu) = base;
		}
		tvec_base_done[cpu] = 1;
	} else {
		base = per_cpu(tvec_bases, cpu);
	}
	
	spin_lock_init(&base->lock);
	
	for (j = 0; j < TVN_SIZE; j++) {
		INIT_LIST_HEAD(base->tv5.vec + j);
		INIT_LIST_HEAD(base->tv4.vec + j);
		INIT_LIST_HEAD(base->tv3.vec + j);
		INIT_LIST_HEAD(base->tv2.vec + j);
	}
	for (j = 0; j < TVR_SIZE; j++)
		INIT_LIST_HEAD(base->tv1.vec + j);
	
	base->timer_jiffies = jiffies;
	base->next_timer = base->timer_jiffies;
	base->active_timers = 0;
	return 0;
}

static inline void detach_timer(struct timer_list *timer, bool clear_pending)
{
	struct list_head *entry = &timer->entry;
	
	__list_del(entry->prev, entry->next);
	if (clear_pending)
		entry->next = NULL;
	entry->prev = LIST_POISON2;
}

static inline void detach_expired_timer(struct timer_list *timer, struct tvec_base *base)
{
	detach_timer(timer, true);
	if (!tbase_get_deferrable(timer->base))
		base->active_timers--;
}

static int detach_if_pending(struct timer_list *timer, struct tvec_base *base,
							 bool clear_pending)
{
	if (!timer_pending(timer))
		return 0;
	
	detach_timer(timer, clear_pending);
	if (!tbase_get_deferrable(timer->base)) {
		base->active_timers--;
		if (timer->expires == base->next_timer)
			base->next_timer = base->timer_jiffies;
	}
	return 1;
}

static void __internal_add_timer(struct tvec_base *base, struct timer_list *timer)
{
	unsigned long expires = timer->expires;
	unsigned long idx = expires - base->timer_jiffies;
	struct list_head *vec;
	
	if (idx < TVR_SIZE) {
		int i = expires & TVR_MASK;
		vec = base->tv1.vec + i;
	} else if (idx < 1 << (TVR_BITS + TVN_BITS)) {
		int i = (expires >> TVR_BITS) & TVN_MASK;
		vec = base->tv2.vec + i;
	} else if (idx < 1 << (TVR_BITS + 2 * TVN_BITS)) {
		int i = (expires >> (TVR_BITS + TVN_BITS)) & TVN_MASK;
		vec = base->tv3.vec + i;
	} else if (idx < 1 << (TVR_BITS + 3 * TVN_BITS)) {
		int i = (expires >> (TVR_BITS + 2 * TVN_BITS)) & TVN_MASK;
		vec = base->tv4.vec + i;
	} else if ((signed long) idx < 0) {
		/*
		 * Can happen if you add a timer with expires == jiffies,
		 * or you set a timer to go off in the past
		 */
		vec = base->tv1.vec + (base->timer_jiffies & TVR_MASK);
	} else {
		int i;
		/* If the timeout is larger than MAX_TVAL (on 64-bit
		 * architectures or with CONFIG_BASE_SMALL=1) then we
		 * use the maximum timeout.
		 */
		if (idx > MAX_TVAL) {
			idx = MAX_TVAL;
			expires = idx + base->timer_jiffies;
		}
		i = (expires >> (TVR_BITS + 3 * TVN_BITS)) & TVN_MASK;
		vec = base->tv5.vec + i;
	}
	/*
	 * Timers are FIFO:
	 */
	list_add_tail(&timer->entry, vec);
}

static void internal_add_timer(struct tvec_base *base, struct timer_list *timer)
{
	__internal_add_timer(base, timer);
	/*
	 * Update base->active_timers and base->next_timer
	 */
	if (!tbase_get_deferrable(timer->base)) {
		if (time_before(timer->expires, base->next_timer))
			base->next_timer = timer->expires;
		base->active_timers++;
	}
}

static int cascade(struct tvec_base *base, struct tvec *tv, int index)
{
	/* cascade all the timers from tv up one level */
	struct timer_list *timer, *tmp;
	struct list_head tv_list;
	
	list_replace_init(tv->vec + index, &tv_list);
	
	/*
	 * We are removing _all_ timers from the list, so we
	 * don't have to detach them individually.
	 */
	list_for_each_entry_safe(timer, tmp, &tv_list, entry) {
		BUG_ON(tbase_get_base(timer->base) != base);
		internal_add_timer(base, timer);
	}
	
	return index;
}

static void call_timer_fn(struct timer_list *timer, void (*fn)(unsigned long),
						  unsigned long data)
{
	int preempt_count = preempt_count();
	
	fn(data);
	
	if (preempt_count != preempt_count()) {
		preempt_count() = preempt_count;
	}
}

#define INDEX(N) ((base->timer_jiffies >> (TVR_BITS + (N) * TVN_BITS)) & TVN_MASK)

/**
 * __run_timers - run all expired timers (if any) on this CPU.
 * @base: the timer vector to be processed.
 *
 * This function cascades all vectors and executes all expired timer
 * vectors.
 */
static inline void __run_timers(struct tvec_base *base)
{
	struct timer_list *timer;
	
	spin_lock_irq(&base->lock);
	while (time_after_eq(jiffies, base->timer_jiffies))
	{
		struct list_head work_list;
		struct list_head *head = &work_list;
		int index = base->timer_jiffies & TVR_MASK;
		
		/*
		 * Cascade timers:
		 */
		if (!index &&
			(!cascade(base, &base->tv2, INDEX(0))) &&
			(!cascade(base, &base->tv3, INDEX(1))) &&
			!cascade(base, &base->tv4, INDEX(2)))
			cascade(base, &base->tv5, INDEX(3));
		
		++base->timer_jiffies;
		list_replace_init(base->tv1.vec + index, &work_list);
		while (!list_empty(head))
		{
			void (*fn)(unsigned long);
			unsigned long data;
			bool irqsafe;
			
			timer = list_first_entry(head, struct timer_list,entry);
			fn = timer->function;
			data = timer->data;
			irqsafe = tbase_get_irqsafe(timer->base);
			
			base->running_timer = timer;
			detach_expired_timer(timer, base);
			
			if (irqsafe) {
				spin_unlock(&base->lock);
				call_timer_fn(timer, fn, data);
				spin_lock(&base->lock);
			} else {
				spin_unlock_irq(&base->lock);
				call_timer_fn(timer, fn, data);
				spin_lock_irq(&base->lock);
			}
		}
	}
	base->running_timer = NULL;
	spin_unlock_irq(&base->lock);
}

static struct tvec_base *lock_timer_base(struct timer_list *timer,
										 unsigned long *flags)
{
	struct tvec_base *base;
	
	for (;;) {
		struct tvec_base *prelock_base = timer->base;
		base = tbase_get_base(prelock_base);
		if (likely(base != NULL)) {
			spin_lock_irqsave(&base->lock, *flags);
			if (likely(prelock_base == timer->base))
				return base;
			/* The timer has migrated to another CPU */
			spin_unlock_irqrestore(&base->lock, *flags);
		}
		cpu_relax();
	}
}

static inline int __mod_timer(struct timer_list *timer, unsigned long expires, bool pending_only, int pinned)
{
	struct tvec_base *base, *new_base;
	unsigned long flags;
	int ret = 0 , cpu;
	
	base = lock_timer_base(timer, &flags);
	
	ret = detach_if_pending(timer, base, false);
	if (!ret && pending_only)
		goto out_unlock;

	cpu = smp_processor_id();
	
#if defined(CONFIG_NO_HZ) && defined(CONFIG_SMP)
	if (!pinned && get_sysctl_timer_migration() && idle_cpu(cpu))
		cpu = get_nohz_timer_target();
#endif
	new_base = per_cpu(tvec_bases, cpu);
	
	if (base != new_base)
	{
		/*
		 * We are trying to schedule the timer on the local CPU.
		 * However we can't change timer's base while it is running,
		 * otherwise del_timer_sync() can't detect that the timer's
		 * handler yet has not finished. This also guarantees that
		 * the timer is serialized wrt itself.
		 */
		if (likely(base->running_timer != timer))
		{
			/* See the comment in lock_timer_base() */
			timer_set_base(timer, NULL);
			spin_unlock(&base->lock);
			base = new_base;
			spin_lock(&base->lock);
			timer_set_base(timer, base);
		}
	}
	
	timer->expires = expires;
	internal_add_timer(base, timer);
	
out_unlock:
	spin_unlock_irqrestore(&base->lock, flags);
	
	return ret;
}

/*
 * Decide where to put the timer while taking the slack into account
 *
 * Algorithm:
 *   1) calculate the maximum (absolute) time
 *   2) calculate the highest bit where the expires and new max are different
 *   3) use this bit to make a mask
 *   4) use the bitmask to round down the maximum time, so that all last
 *      bits are zeros
 */
static inline unsigned long apply_slack(struct timer_list *timer, unsigned long expires)
{
	unsigned long expires_limit, mask;
	int bit;
	
	if (timer->slack >= 0) {
		expires_limit = expires + timer->slack;
	} else {
		long delta = expires - jiffies;
		
		if (delta < 256)
			return expires;
		
		expires_limit = expires + delta / 256;
	}
	mask = expires ^ expires_limit;
	if (mask == 0)
		return expires;
	
	bit = find_last_bit(&mask, BITS_PER_LONG);
	
	mask = (1 << bit) - 1;
	
	expires_limit = expires_limit & ~(mask);
	
	return expires_limit;
}

static void run_timer_softirq(struct dpc_irq_action *h)
{
	struct tvec_base *base = __this_cpu_read(tvec_bases);
	
	//TODO: to support hrtimer
	if (time_after_eq(jiffies, base->timer_jiffies))
		__run_timers(base);
}

void run_local_timers(void)
{
	//TODO: to support hrtimer
	raise_dpc_irq(TIMER_SOFTIRQ);
}

void __init hal_timer_init(void)
{
	/* 目前没有走多CPU的初始化机制，直接手动给第一个CPU分配tvec_base */
	init_timers_cpu(0);
	
	/* 向CPU子系统注册回调函数，主要是CPU启动建立tvec_base，CPU停掉时迁移tvec_base等 */
	//TODO 
	
	open_dpc_irq(TIMER_SOFTIRQ, run_timer_softirq);
}

/*********************************************************************
 Export interface
*********************************************************************/
int hal_timer_mod(struct timer_list *timer, unsigned long expires)
{
	expires = apply_slack(timer, expires);
	
	if (timer_pending(timer) && timer->expires == expires)
	{
		return 1;
	}
	
	return __mod_timer(timer, expires, false, TIMER_NOT_PINNED);
}

void hal_timer_add(struct timer_list *timer)
{
	if (timer_pending(timer))
		return;
	hal_timer_mod(timer, timer->expires);
}

int hal_timer_del(struct timer_list *timer)
{
	struct tvec_base *base;
	unsigned long flags;
	int ret = 0;
	
	if (timer_pending(timer)) {
		base = lock_timer_base(timer, &flags);
		ret = detach_if_pending(timer, base, true);
		spin_unlock_irqrestore(&base->lock, flags);
	}
	
	return ret;
}


/**
	@brief Sleep milliseconds
 
	休眠milliseconds并且不退出休眠直到时间流逝完
 */
void hal_msleep(unsigned int msecs)
{
	//TODO: msleep
}

/**
	@brief Sleep in us unit
 
	由于US的单位比较小，线程不一定进入休眠状态，可能进入轮询状态
 */
void hal_usleep(unsigned int us)
{
	//TODO:
}
