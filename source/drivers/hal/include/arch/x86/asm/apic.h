/*
	APIC 组件
*/
#ifndef ARCH_APIC_H
#define ARCH_APIC_H

#include <percpu.h>
#include <cpumask.h>
#include <debug.h>

#include "fixmap.h"
#include "apicdef.h"
#include "processor.h"
#include "alternative.h"
#include "smp.h"

/* macro */
#define apic_printk(v, s, a...) do {		\
			hal_printf("\nAPIC信息：");		\
			hal_printf(s, ##a);					\
	} while (0)

//apic.c
extern unsigned long mp_lapic_addr ;
extern unsigned int boot_cpu_physical_apicid ;
extern unsigned disabled_cpus;
extern int pic_mode;
extern int smp_found_config;
extern int apic_version[MAX_LOCAL_APIC];

void __cpuinit generic_processor_info( int apicid, int version );
void __init connect_bsp_APIC( void );
void __cpuinit setup_local_APIC( void );
void __cpuinit bsp_end_local_APIC_setup();
void __init register_lapic_address( unsigned long address );

/************************************************************************/
/* APIC STRUCTURE                                                       */
/************************************************************************/
struct apic {
	const char *name;
	
	/* Init */
	int (*probe)(void);
	void (*init_apic_ldr)(void);

	/* apic ops */
	u32		(*read)( u32 reg );
	void	(*write)( u32 reg, u32 v );
	u64		(*icr_read)( void );
	void	(*icr_write)( u32 low, u32 high );
	void	(*wait_icr_idle)( void );
	u32		(*safe_wait_icr_idle)( void );
	int		(*cpu_present_to_apicid)(int mps_cpu);
	void	(*enable_apic_mode)(void);

	int		(*phys_pkg_id)(int cpuid_apic, int index_msb);
	unsigned int (*get_apic_id)(unsigned long x);

	/* wakeup_secondary_cpu */
	int (*wakeup_secondary_cpu)(int apicid, unsigned long start_eip);

	/* data */
	int disable_esr;
	
};
extern struct apic *apic;

/************************************************************************/
/* APIC IO                                                              */
/************************************************************************/
#ifdef CONFIG_X86_LOCAL_APIC
#define APIC_DFR_VALUE	(APIC_DFR_FLAT)

static inline u32 native_apic_mem_read(u32 reg)
{
	return *((volatile u32 *)(APIC_BASE + reg));
}
static inline void native_apic_mem_write(u32 reg, u32 v)
{
	volatile u32 *addr = (volatile u32 *)(APIC_BASE + reg);

	alternative_io("movl %0, %1", "xchgl %0, %1", X86_FEATURE_11AP,
		ASM_OUTPUT2("=r" (v), "=m" (*addr)),
		ASM_OUTPUT2("0" (v), "m" (*addr)));
}

static inline u32 apic_read(u32 reg)
{
	return apic->read(reg);
}

static inline void apic_write(u32 reg, u32 val)
{
	apic->write(reg, val);
}

static inline u64 apic_icr_read(void)
{
	return apic->icr_read();
}

static inline void apic_icr_write(u32 low, u32 high)
{
	apic->icr_write(low, high);
}

static inline void apic_wait_icr_idle(void)
{
	apic->wait_icr_idle();
}

static inline u32 safe_apic_wait_icr_idle(void)
{
	return apic->safe_wait_icr_idle();
}

extern void native_apic_wait_icr_idle(void);
extern u32 native_safe_apic_wait_icr_idle(void);
extern void native_apic_icr_write(u32 low, u32 id);
extern u64 native_apic_icr_read(void);

static inline unsigned int read_apic_id(void)
{
	unsigned int reg;

	reg = apic_read(APIC_ID);

	return apic->get_apic_id(reg);
}

#else /* CONFIG_X86_LOCAL_APIC */

static inline u32 apic_read(u32 reg) { return 0; }
static inline void apic_write(u32 reg, u32 val) { }
static inline u64 apic_icr_read(void) { return 0; }
static inline void apic_icr_write(u32 low, u32 high) { }
static inline void apic_wait_icr_idle(void) { }
static inline u32 safe_apic_wait_icr_idle(void) { return 0; }

#endif /* CONFIG_X86_LOCAL_APIC */

/************************************************************************/
/* APIC for CPU ID                                                      */
/************************************************************************/
#ifdef CONFIG_X86_LOCAL_APIC
static inline int __default_cpu_present_to_apicid(int mps_cpu)
{
	if (mps_cpu < nr_cpu_ids && cpu_present(mps_cpu))
		return (int)per_cpu(x86_bios_cpu_apicid, mps_cpu);
	else
		return BAD_APICID;
}

#ifdef CONFIG_X86_32
/*
* Set up the logical destination ID.
*
* Intel recommends to set DFR, LDR and TPR before enabling
* an APIC.  See e.g. "AP-388 82489DX User's Manual" (Intel
* document number 292116).  So here it goes...
*/
extern void default_init_apic_ldr(void);

static inline int default_phys_pkg_id(int cpuid_apic, int index_msb)
{
	return cpuid_apic >> index_msb;
}
static inline int default_cpu_present_to_apicid(int mps_cpu)
{
	return __default_cpu_present_to_apicid(mps_cpu);
}

#else 
#error "x86-64 cpu id ops should be defined."
#endif /* CONFIG_X86_32 */
#endif /* CONFIG_X86_LOCAL_APIC */

static inline unsigned default_get_apic_id(unsigned long x)
{
	unsigned int ver = GET_APIC_VERSION(apic_read(APIC_LVR));

	if (APIC_XAPIC(ver) || boot_cpu_has(X86_FEATURE_EXTD_APICID))
		return (x >> 24) & 0xFF;
	else
		return (x >> 24) & 0x0F;
}

/************************************************************************/
/* COMMON for 32/64                                                     */
/************************************************************************/
void clear_local_APIC(void);
void init_bsp_APIC();
int lapic_get_maxlvt(void);

static inline void ack_APIC_irq(void)
{
	/*
	* ack_APIC_irq() actually gets compiled as a single instruction
	* ... yummie.
	*/

	/* Docs say use 0 for future compatibility */
	apic_write(APIC_EOI, 0);
}

#endif
