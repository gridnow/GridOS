#ifndef ASM_GLUE_PROC_H
#define ASM_GLUE_PROC_H

#include <asm/glue.h>

/*
 * Work out if we need multiple CPU support
 */
#undef MULTI_CPU
#undef CPU_NAME

#if defined(CONFIG_CPU_V6) || defined(CONFIG_CPU_V6K)
# ifdef CPU_NAME
#  undef  MULTI_CPU
#  define MULTI_CPU
# else
#  define CPU_NAME cpu_v6
# endif
#endif

#ifdef CONFIG_CPU_V7
# ifdef CPU_NAME
#  undef  MULTI_CPU
#  define MULTI_CPU
# else
#  define CPU_NAME cpu_v7
# endif
#endif

#ifndef MULTI_CPU
#define cpu_proc_init			__glue(CPU_NAME,_proc_init)
#define cpu_proc_fin			__glue(CPU_NAME,_proc_fin)
#define cpu_reset			__glue(CPU_NAME,_reset)
#define cpu_do_idle			__glue(CPU_NAME,_do_idle)
#define cpu_dcache_clean_area		__glue(CPU_NAME,_dcache_clean_area)
#define cpu_do_switch_mm		__glue(CPU_NAME,_switch_mm)
#define cpu_set_pte_ext			__glue(CPU_NAME,_set_pte_ext)
#define cpu_suspend_size		__glue(CPU_NAME,_suspend_size)
#define cpu_do_suspend			__glue(CPU_NAME,_do_suspend)
#define cpu_do_resume			__glue(CPU_NAME,_do_resume)
#endif

#endif
