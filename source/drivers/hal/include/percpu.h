/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Ã¿CPU±äÁ¿
*/

#ifndef HAL_PCPU_H
#define HAL_PCPU_H

#ifdef CONFIG_SMP
#warning "Percpu should be supported on smp, now we #0 cpu is used"
#endif
#define per_cpu(v, cpu) v

#endif /* PCPU_H */