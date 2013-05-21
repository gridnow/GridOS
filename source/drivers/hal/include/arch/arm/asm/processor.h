#ifndef __ASM_ARM_PROCESSOR_H
#define __ASM_ARM_PROCESSOR_H

/*
 * Default implementation of macro that returns current
 * instruction pointer ("program counter").
 */
#define current_text_addr() ({ __label__ _l; _l: &&_l;})

struct thread_struct {
							/* fault info	  */
	unsigned long		address;
	unsigned long		trap_no;
	unsigned long		error_code;
};

#if __LINUX_ARM_ARCH__ == 6 || defined(CONFIG_ARM_ERRATA_754327)
#define cpu_relax()			smp_mb()
#else
#define cpu_relax()			barrier()
#endif

/*
 * Prefetching support - only ARMv5.
 */
#if __LINUX_ARM_ARCH__ >= 5

#define ARCH_HAS_PREFETCH
static inline void prefetch(const void *ptr)
{
	__asm__ __volatile__(
		"pld\t%a0"
		:
		: "p" (ptr)
		: "cc");
}

#define ARCH_HAS_PREFETCHW
#define prefetchw(ptr)	prefetch(ptr)

#define ARCH_HAS_SPINLOCK_PREFETCH
#define spin_lock_prefetch(x) do { } while (0)

#endif /* Prefetching */

#define HAVE_ARCH_PICK_MMAP_LAYOUT

#endif /* __ASM_ARM_PROCESSOR_H */
