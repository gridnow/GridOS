/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HAL模块配置
*/

/* Should be genereated and some information should meet kernel */
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

/* config of x86 */
#if defined(__i386__)
#define CONFIG_HAL_KERNEL_BASE				0x80000000
#define CONFIG_HAL_KERNEL_MEM_LEN			0x40000000
#define CONFIG_X86_L1_CACHE_SHIFT			6								//from the generated from of autoconf.h
#define CONFIG_X86_LOCAL_APIC
#define CONFIG_X86_32
#define PAGE_SHIFT							12								//4kb page
#define CONFIG_GENERIC_FIND_FIRST_BIT
#define CONFIG_GENERIC_HARDIRQS               1
#elif defined(__mips__)
#define CONFIG_HAL_KERNEL_BASE				0xFFFFFFFF80000000UL
#define CONFIG_GENERIC_FIND_FIRST_BIT
#else
#error "config.h of hal is invalid"
#endif

/* config for loongson */
#ifdef __mips__
//#define CONFIG_CPU_LOONGSON2				1								//龙芯2系列很多东西是特别的，3A时要取消掉该选项。
#define CONFIG_64BIT						1								//64位
#define CONFIG_PAGE_SIZE_16KB				1								//asm/mipsregs.h需要
#define PAGE_SHIFT							14								//16kb PAGE
#define CONFIG_WEAK_REORDERING_BEYOND_LLSC	1								//mips/asm/barrier.h中，多处理间需要用
#define CONFIG_WEAK_ORDERING				1								//mips/asm/barrier.h中，多处理间需要用
#define CONFIG_CPU_HAS_SYNC					1								//mips CPU 有SYNC指令，barrier需要
#define CONFIG_CPU_HAS_WB					1								//loongson CPU 有WBFLUSH功能，其实SYNC指令具备该功能，在barrier中需要
#define CONFIG_I8259						1	
#define CONFIG_IRQ_CPU						1								//CPU 继承了8个终端源
#define CONFIG_MIPS_L1_CACHE_SHIFT			5								//32 LINE SIZE
#define CONFIG_GENERIC_HARDIRQS             1         
#define CONFIG_BUG							1
#define CONFIG_CPU_LOONGSON					1								// kmm_arch_ctx_init_kernel and TLB flush need it
#include <asm/asm-offsets.h>
#endif

/* config of common */
#define CONFIG_SMP							1
#define NR_CPUS								256
#define NR_IRQS								128								//IRQ/IRQDESC.c需要一个全局数组描述系统中最多的中断描述符
#define CONFIG_NR_CPUS						NR_CPUS
#define PAGE_SIZE							(1UL << PAGE_SHIFT)
#define PAGE_MASK							(~((1 << PAGE_SHIFT) - 1))
#define CONFIG_HZ							250
#define CONFIG_GENERIC_CLOCKEVENTS_BUILD	1								/* Clockchips.h 需要 */
#endif /* HAL_CONFIG_H */
