/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   CPU Mask
*/
#ifndef HAL_CPUMASK_H
#define HAL_CPUMASK_H

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

#endif

