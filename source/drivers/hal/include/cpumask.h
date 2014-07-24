/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPU Mask
*/
#ifndef HAL_CPUMASK_H
#define HAL_CPUMASK_H

#include <bitops.h>

#if NR_CPUS == 1
#define nr_cpu_ids		1
#else
extern int nr_cpu_ids;
#endif

#ifdef CONFIG_SMP
#define cpu_present(cpu)	((cpu) == 0)
#warning "CPU Mask in smp should be supported"
#else
#define cpu_present(cpu)	((cpu) == 0)
#endif

typedef struct cpumask { DECLARE_BITMAP(bits, NR_CPUS); } cpumask_t;

/*
	We now put on stack for small cpu count
*/
#if (NR_CPUS > 32)
#error "cpumask_var_t may overflow"
#endif
typedef struct cpumask cpumask_var_t[1];

/* verify cpu argument to cpumask_* operators */
static inline unsigned int cpumask_check(unsigned int cpu)
{
	return cpu;
}

#define cpumask_bits(maskp) ((maskp)->bits)

#define cpumask_test_cpu(cpu, cpumask) \
	test_bit(cpumask_check(cpu), cpumask_bits((cpumask)))

/***********************************************************
	About Memory struct and memory mask.
***********************************************************/
/* For flush compatible, should move out */
struct mm_struct
{
	unsigned int asid;
	cpumask_var_t cpu_vm_mask_var;
};
struct vm_area_struct
{
	struct mm_struct *vm_mm;
};

/* Future-safe accessor for struct mm_struct's cpu_vm_mask. */
static inline cpumask_t *mm_cpumask(struct mm_struct *mm)
{
	return mm->cpu_vm_mask_var;
}
#endif

