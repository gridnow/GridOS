/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL Ã¿CPU±äÁ¿
*/

#ifndef HAL_PCPU_H
#define HAL_PCPU_H

#define per_cpu(v, cpu) v
#define __this_cpu_read(pcp) pcp
#define __raw_get_cpu_var(var) var
#endif /* PCPU_H */