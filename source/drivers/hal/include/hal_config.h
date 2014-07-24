/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   HALģ������
*/

/* Should be genereated and some information should meet kernel */
#ifndef HAL_CONFIG_H
#define HAL_CONFIG_H

#if defined(__i386__)
#define CONFIG_HAL_KERNEL_BASE				0x80000000UL
#define CONFIG_HAL_KERNEL_MEM_LEN			0x40000000						//�ں�ֱ��ӳ�䳤��
#define CONFIG_HAL_KERNEL_VM_LEN			0X40000000						//�����ڴ泤��
#define CONFIG_X86_L1_CACHE_SHIFT			6								//from the generated from of autoconf.h
#define CONFIG_X86_INTERNODE_CACHE_SHIFT	6								//�ڵ�֮�乲���ڴ�Ķ���.
#define CONFIG_X86_LOCAL_APIC				1
#define CONFIG_X86_32						1
#define PAGE_SHIFT							12								//4kb page
#define CONFIG_GENERIC_FIND_FIRST_BIT		1
#define CONFIG_RTC							1								

#elif defined(__mips__)
#define CONFIG_HAL_KERNEL_BASE				0xFFFFFFFF80000000UL
#define CONFIG_GENERIC_FIND_FIRST_BIT
#define CONFIG_RTC							1

#elif defined(__arm__)
#define CONFIG_HAL_KERNEL_BASE				0xc0000000						//�ں���ʼ��ַ
#define CONFIG_HAL_KERNEL_MEM_LEN			0x10000000						//�ں�ֱ��ӳ�䳤��
#define CONFIG_HAL_KERNEL_VM_LEN			0x10000000
#define CONFIG_CPU_USE_DOMAINS				1
#define CONFIG_CPU_HAS_ASID					1
#define CONFIG_CPU_TLB_V6					1
#define CONFIG_CPU_V6						1
#define CONFIG_CPU_32v6K					1								/* spinlock need it*/
#define	CONFIG_MMU							1
#define PHYS_OFFSET							0x50000000						//��ͬ��ƽ̨��һ�����˴�ҪŲ��
#define PAGE_OFFSET							CONFIG_HAL_KERNEL_BASE
#define PAGE_SZ								(1<<PAGE_SHIFT)					//�����
#define PAGE_SHIFT							12
#define __LINUX_ARM_ARCH__					6
#define CONFIG_ARM_L1_CACHE_SHIFT			5								//From the generated
#define CONFIG_AEABI						1								//Use EABI

#else
#error "config.h of hal is invalid"
#endif

/* config for loongson */
#ifdef __mips__
//#define CONFIG_CPU_LOONGSON2				1								//��о2ϵ�кܶණ�����ر�ģ�3AʱҪȡ������ѡ�
#define CONFIG_64BIT						1								//64λ
#define CONFIG_PAGE_SIZE_16KB				1								//asm/mipsregs.h��Ҫ
#define PAGE_SHIFT							14								//16kb PAGE
#define CONFIG_WEAK_REORDERING_BEYOND_LLSC	1								//mips/asm/barrier.h�У��ദ�����Ҫ��
#define CONFIG_WEAK_ORDERING				1								//mips/asm/barrier.h�У��ദ�����Ҫ��
#define CONFIG_CPU_HAS_SYNC					1								//mips CPU ��SYNCָ�barrier��Ҫ
#define CONFIG_CPU_HAS_WB					1								//loongson CPU ��WBFLUSH���ܣ���ʵSYNCָ��߱��ù��ܣ���barrier����Ҫ
#define CONFIG_I8259						1	
#define CONFIG_IRQ_CPU						1								//CPU �̳���8���ն�Դ
#define CONFIG_MIPS_L1_CACHE_SHIFT			5								//32 LINE SIZE         
#define CONFIG_BUG							1
#define CONFIG_CPU_LOONGSON					1								// kmm_arch_ctx_init_kernel and TLB flush need it
#include <asm/asm-offsets.h>
#endif

/* Config of common */
#define CONFIG_SMP							1
#define NR_CPUS								32
#define NR_IRQS								128								//IRQ/IRQDESC.c��Ҫһ��ȫ����������ϵͳ�������ж�������
#define CONFIG_NR_CPUS						NR_CPUS
#define PAGE_SIZE							(1UL << PAGE_SHIFT)
#define PAGE_MASK							(~(PAGE_SIZE - 1))
#define CONFIG_HZ							250
#define CONFIG_GENERIC_CLOCKEVENTS_BUILD	1								/* Clockchips.h ��Ҫ */
#define CONFIG_CLKEVT_I8253					1								/* 8253 ������Ϊʱ���¼� */
//#define CONFIG_CLKSRC_I8253					1								/* 8253 ������Ϊʱ��Դ */
#define CONFIG_GENERIC_HARDIRQS             1

/* Basic HAL memory conversion */
#ifndef PHYS_OFFSET															/* No physical offset means physical address starting from 0 */
#define PHYS_OFFSET 0
#endif
#define HAL_GET_BASIC_KADDRESS(PHY) ((PHY) - PHYS_OFFSET + CONFIG_HAL_KERNEL_BASE)
#define HAL_GET_BASIC_PHYADDRESS(LOG) ((unsigned long)(LOG) - CONFIG_HAL_KERNEL_BASE + PHYS_OFFSET)

#endif /* HAL_CONFIG_H */
