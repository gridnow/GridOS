/*
 * Defines x86 CPU feature bits
 */
#ifndef _ASM_X86_CPUFEATURE_H
#define _ASM_X86_CPUFEATURE_H

#include "required-features.h"
#include "cpufeature_def.h"
#include <asm/processor.h>
#include <asm/bitops.h>


#define test_cpu_cap(c, bit)						\
	 test_bit(bit, (unsigned long *)((c)->x86_capability))

#define cpu_has(c, bit)							\
	(__builtin_constant_p(bit) &&					\
	 ( (((bit)>>5)==0 && (1UL<<((bit)&31) & REQUIRED_MASK0)) ||	\
	   (((bit)>>5)==1 && (1UL<<((bit)&31) & REQUIRED_MASK1)) ||	\
	   (((bit)>>5)==2 && (1UL<<((bit)&31) & REQUIRED_MASK2)) ||	\
	   (((bit)>>5)==3 && (1UL<<((bit)&31) & REQUIRED_MASK3)) ||	\
	   (((bit)>>5)==4 && (1UL<<((bit)&31) & REQUIRED_MASK4)) ||	\
	   (((bit)>>5)==5 && (1UL<<((bit)&31) & REQUIRED_MASK5)) ||	\
	   (((bit)>>5)==6 && (1UL<<((bit)&31) & REQUIRED_MASK6)) ||	\
	   (((bit)>>5)==7 && (1UL<<((bit)&31) & REQUIRED_MASK7)) ||	\
	   (((bit)>>5)==8 && (1UL<<((bit)&31) & REQUIRED_MASK8)) ||	\
	   (((bit)>>5)==9 && (1UL<<((bit)&31) & REQUIRED_MASK9)) )	\
	  ? 1 :								\
	 test_cpu_cap(c, bit))

#define boot_cpu_has(bit)	cpu_has(&boot_cpu_data, bit)

#define set_cpu_cap(c, bit)	set_bit(bit, (unsigned long *)((c)->x86_capability))
#define clear_cpu_cap(c, bit)	clear_bit(bit, (unsigned long *)((c)->x86_capability))
#define setup_clear_cpu_cap(bit) do { \
	clear_cpu_cap(&boot_cpu_data, bit);	\
	set_bit(bit, (unsigned long *)cpu_caps_cleared); \
} while (0)
#define setup_force_cpu_cap(bit) do { \
	set_cpu_cap(&boot_cpu_data, bit);	\
	set_bit(bit, (unsigned long *)cpu_caps_set);	\
} while (0)

#define cpu_has_fpu		boot_cpu_has(X86_FEATURE_FPU)
#define cpu_has_vme		boot_cpu_has(X86_FEATURE_VME)
#define cpu_has_de		boot_cpu_has(X86_FEATURE_DE)
#define cpu_has_pse		boot_cpu_has(X86_FEATURE_PSE)
#define cpu_has_tsc		boot_cpu_has(X86_FEATURE_TSC)
#define cpu_has_pae		boot_cpu_has(X86_FEATURE_PAE)
#define cpu_has_pge		boot_cpu_has(X86_FEATURE_PGE)
#define cpu_has_apic		boot_cpu_has(X86_FEATURE_APIC)
#define cpu_has_sep		boot_cpu_has(X86_FEATURE_SEP)
#define cpu_has_mtrr		boot_cpu_has(X86_FEATURE_MTRR)
#define cpu_has_mmx		boot_cpu_has(X86_FEATURE_MMX)
#define cpu_has_fxsr		boot_cpu_has(X86_FEATURE_FXSR)
#define cpu_has_xmm		boot_cpu_has(X86_FEATURE_XMM)
#define cpu_has_xmm2		boot_cpu_has(X86_FEATURE_XMM2)
#define cpu_has_xmm3		boot_cpu_has(X86_FEATURE_XMM3)
#define cpu_has_aes		boot_cpu_has(X86_FEATURE_AES)
#define cpu_has_ht		boot_cpu_has(X86_FEATURE_HT)
#define cpu_has_mp		boot_cpu_has(X86_FEATURE_MP)
#define cpu_has_nx		boot_cpu_has(X86_FEATURE_NX)
#define cpu_has_k6_mtrr		boot_cpu_has(X86_FEATURE_K6_MTRR)
#define cpu_has_cyrix_arr	boot_cpu_has(X86_FEATURE_CYRIX_ARR)
#define cpu_has_centaur_mcr	boot_cpu_has(X86_FEATURE_CENTAUR_MCR)
#define cpu_has_xstore		boot_cpu_has(X86_FEATURE_XSTORE)
#define cpu_has_xstore_enabled	boot_cpu_has(X86_FEATURE_XSTORE_EN)
#define cpu_has_xcrypt		boot_cpu_has(X86_FEATURE_XCRYPT)
#define cpu_has_xcrypt_enabled	boot_cpu_has(X86_FEATURE_XCRYPT_EN)
#define cpu_has_ace2		boot_cpu_has(X86_FEATURE_ACE2)
#define cpu_has_ace2_enabled	boot_cpu_has(X86_FEATURE_ACE2_EN)
#define cpu_has_phe		boot_cpu_has(X86_FEATURE_PHE)
#define cpu_has_phe_enabled	boot_cpu_has(X86_FEATURE_PHE_EN)
#define cpu_has_pmm		boot_cpu_has(X86_FEATURE_PMM)
#define cpu_has_pmm_enabled	boot_cpu_has(X86_FEATURE_PMM_EN)
#define cpu_has_ds		boot_cpu_has(X86_FEATURE_DS)
#define cpu_has_pebs		boot_cpu_has(X86_FEATURE_PEBS)
#define cpu_has_clflush		boot_cpu_has(X86_FEATURE_CLFLSH)
#define cpu_has_bts		boot_cpu_has(X86_FEATURE_BTS)
#define cpu_has_gbpages		boot_cpu_has(X86_FEATURE_GBPAGES)
#define cpu_has_arch_perfmon	boot_cpu_has(X86_FEATURE_ARCH_PERFMON)
#define cpu_has_pat		boot_cpu_has(X86_FEATURE_PAT)
#define cpu_has_xmm4_1		boot_cpu_has(X86_FEATURE_XMM4_1)
#define cpu_has_xmm4_2		boot_cpu_has(X86_FEATURE_XMM4_2)
#define cpu_has_x2apic		boot_cpu_has(X86_FEATURE_X2APIC)
#define cpu_has_xsave		boot_cpu_has(X86_FEATURE_XSAVE)
#define cpu_has_hypervisor	boot_cpu_has(X86_FEATURE_HYPERVISOR)
#define cpu_has_pclmulqdq	boot_cpu_has(X86_FEATURE_PCLMULQDQ)
#define cpu_has_perfctr_core	boot_cpu_has(X86_FEATURE_PERFCTR_CORE)
#define cpu_has_cx8		boot_cpu_has(X86_FEATURE_CX8)
#define cpu_has_cx16		boot_cpu_has(X86_FEATURE_CX16)
#define cpu_has_eager_fpu	boot_cpu_has(X86_FEATURE_EAGER_FPU)
#define cpu_has_topoext		boot_cpu_has(X86_FEATURE_TOPOEXT)

#if defined(CONFIG_X86_INVLPG) || defined(CONFIG_X86_64)
# define cpu_has_invlpg		1
#else
# define cpu_has_invlpg		(boot_cpu_data.x86 > 3)
#endif

#ifdef CONFIG_X86_64

#undef  cpu_has_vme
#define cpu_has_vme		0

#undef  cpu_has_pae
#define cpu_has_pae		___BUG___

#undef  cpu_has_mp
#define cpu_has_mp		1

#undef  cpu_has_k6_mtrr
#define cpu_has_k6_mtrr		0

#undef  cpu_has_cyrix_arr
#define cpu_has_cyrix_arr	0

#undef  cpu_has_centaur_mcr
#define cpu_has_centaur_mcr	0

#endif /* CONFIG_X86_64 */

#endif /* _ASM_X86_CPUFEATURE_H */
