/*
	CPU 探测和初始化框架
	wuxin
*/

#include <types.h>
#include <string.h>
#include <irqflags.h>

#include <asm/processor.h>
#include <asm/processor-flags.h>
#include <asm/bitops.h>
#include <asm/hypervisor.h>
#include <asm/mce.h>
#include <asm/msr.h>
#include <asm/segment.h>
#include <asm/desc.h>
#include <asm/system.h>
#include <asm/i387.h>

#include "cpu.h"

static void __cpuinit default_init(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_64
	cpu_detect_cache_sizes(c);
#else
	/* Not much we can do here... */
	/* Check if at least it has cpuid */
	if (c->cpuid_level == -1) {
		/* No cpuid. It must be an ancient CPU */
		if (c->x86 == 4)
			strcpy(c->x86_model_id, "486");
		else if (c->x86 == 3)
			strcpy(c->x86_model_id, "386");
	}
#endif
}

static const struct cpu_dev __cpuinitconst default_cpu = {
	.c_init		= default_init,
	.c_vendor	= "Unknown",
	.c_x86_vendor	= X86_VENDOR_UNKNOWN,
};
static const struct cpu_dev *this_cpu __cpuinitdata = &default_cpu;

#if !defined(CONFIG_X86_PAE) || defined(CONFIG_X86_64)
unsigned long mmu_cr4_features;
#else
unsigned long mmu_cr4_features = X86_CR4_PAE;
#endif

#ifdef CONFIG_X86_32
/* cpu data as detected by the assembly code in head.S */
struct cpuinfo_x86 new_cpu_data __cpuinitdata = {0, 0, 0, 0, -1, 1, 0, 0, -1};
/* common cpu data for all cpus */
struct cpuinfo_x86 boot_cpu_data __read_mostly = {0, 0, 0, 0, -1, 1, 0, 0, -1};

/* User set cache size */
static int cachesize_override __cpuinitdata = -1;
static int disable_x86_serial_nr __cpuinitdata = 1;

/* Standard macro to see if a specific flag is changeable */
static inline int flag_is_changeable_p(u32 flag)
{
	u32 f1, f2;

	/*
	 * Cyrix and IDT cpus allow disabling of CPUID
	 * so the code below may return different results
	 * when it is executed before and after enabling
	 * the CPUID. Add "volatile" to not allow gcc to
	 * optimize the subsequent calls to this function.
	 */
	asm volatile ("pushfl		\n\t"
		      "pushfl		\n\t"
		      "popl %0		\n\t"
		      "movl %0, %1	\n\t"
		      "xorl %2, %0	\n\t"
		      "pushl %0		\n\t"
		      "popfl		\n\t"
		      "pushfl		\n\t"
		      "popl %0		\n\t"
		      "popfl		\n\t"

		      : "=&r" (f1), "=&r" (f2)
		      : "ir" (flag));

	return ((f1^f2) & flag) != 0;
}

/* Probe for the CPUID instruction */
static int __cpuinit have_cpuid_p(void)
{
	return flag_is_changeable_p(X86_EFLAGS_ID);
}

static void __cpuinit squash_the_stupid_serial_number(struct cpuinfo_x86 *c)
{
	unsigned long lo, hi;

	if (!cpu_has(c, X86_FEATURE_PN) || !disable_x86_serial_nr)
		return;

	/* Disable processor serial number: */

	rdmsr(MSR_IA32_BBL_CR_CTL, lo, hi);
	lo |= 0x200000;
	wrmsr(MSR_IA32_BBL_CR_CTL, lo, hi);

	hal_printf_warning("CPU serial number disabled.\n");
	clear_cpu_cap(c, X86_FEATURE_PN);

	/* Disabling the serial number may affect the cpuid level */
	c->cpuid_level = cpuid_eax(0);
}
#endif

static int disable_smep __cpuinitdata;
static __cpuinit void setup_smep(struct cpuinfo_x86 *c)
{
	if (cpu_has(c, X86_FEATURE_SMEP)) {
		if (unlikely(disable_smep)) {
			setup_clear_cpu_cap(X86_FEATURE_SMEP);
			clear_in_cr4(X86_CR4_SMEP);
		} else
			set_in_cr4(X86_CR4_SMEP);
	}
}

static __always_inline void setup_smap(struct cpuinfo_x86 *c)
{
	unsigned long eflags;

	/* This should have been cleared long ago */
	raw_local_save_flags(eflags);
	BUG_ON(eflags & X86_EFLAGS_AC);

	if (cpu_has(c, X86_FEATURE_SMAP))
		set_in_cr4(X86_CR4_SMAP);
}

static const struct cpu_dev *__cpuinitdata cpu_devs[X86_VENDOR_NUM] = {};
/*
 * Some CPU features depend on higher CPUID levels, which may not always
 * be available due to CPUID level capping or broken virtualization
 * software.  Add those features to this table to auto-disable them.
 */
struct cpuid_dependent_feature {
	u32 feature;
	u32 level;
};

static const struct cpuid_dependent_feature __cpuinitconst
cpuid_dependent_features[] = {
	{ X86_FEATURE_MWAIT,		0x00000005 },
	{ X86_FEATURE_DCA,		0x00000009 },
	{ X86_FEATURE_XSAVE,		0x0000000d },
	{ 0, 0 }
};

static void __cpuinit filter_cpuid_features(struct cpuinfo_x86 *c, bool warn)
{
	const struct cpuid_dependent_feature *df;

	for (df = cpuid_dependent_features; df->feature; df++) {

		if (!cpu_has(c, df->feature))
			continue;
		/*
		 * Note: cpuid_level is set to -1 if unavailable, but
		 * extended_extended_level is set to 0 if unavailable
		 * and the legitimate extended levels are all negative
		 * when signed; hence the weird messing around with
		 * signs here...
		 */
		if (!((s32)df->level < 0 ?
		     (u32)df->level > (u32)c->extended_cpuid_level :
		     (s32)df->level > (s32)c->cpuid_level))
			continue;

		clear_cpu_cap(c, df->feature);
		if (!warn)
			continue;

		hal_printf_warning( "CPU: CPU feature %x disabled, no CPUID level 0x%x\n",
				df->feature, df->level);
	}
}

/*
 * Naming convention should be: <Name> [(<Codename>)]
 * This table only is used unless init_<vendor>() below doesn't set it;
 * in particular, if CPUID levels 0x80000002..4 are supported, this
 * isn't used
 */

/* Look up CPU names by table lookup. */
static const char *__cpuinit table_lookup_model(struct cpuinfo_x86 *c)
{
	const struct cpu_model_info *info;

	if (c->x86_model >= 16)
		return NULL;	/* Range check */

	if (!this_cpu)
		return NULL;

	info = this_cpu->c_models;

	while (info && info->family) {
		if (info->family == c->x86)
			return info->model_names[c->x86_model];
		info++;
	}
	return NULL;		/* Not found */
}

__u32 cpu_caps_cleared[NCAPINTS] __cpuinitdata;
__u32 cpu_caps_set[NCAPINTS] __cpuinitdata;

static void __cpuinit get_model_name(struct cpuinfo_x86 *c)
{
	unsigned int *v;
	char *p, *q;

	if (c->extended_cpuid_level < 0x80000004)
		return;

	v = (unsigned int *)c->x86_model_id;
	cpuid(0x80000002, &v[0], &v[1], &v[2], &v[3]);
	cpuid(0x80000003, &v[4], &v[5], &v[6], &v[7]);
	cpuid(0x80000004, &v[8], &v[9], &v[10], &v[11]);
	c->x86_model_id[48] = 0;

	/*
	* Intel chips right-justify this string for some dumb reason;
	* undo that brain damage:
	*/
	p = q = &c->x86_model_id[0];
	while (*p == ' ')
		p++;
	if (p != q) {
		while (*p)
			*q++ = *p++;
		while (q <= &c->x86_model_id[48])
			*q++ = '\0';	/* Zero-pad the rest */
	}
}

void __cpuinit cpu_detect_cache_sizes(struct cpuinfo_x86 *c)
{
	unsigned int n, dummy, ebx, ecx, edx, l2size;

	n = c->extended_cpuid_level;

	if (n >= 0x80000005) {
		cpuid(0x80000005, &dummy, &ebx, &ecx, &edx);
		c->x86_cache_size = (ecx>>24) + (edx>>24);
#ifdef CONFIG_X86_64
		/* On K8 L1 TLB is inclusive, so don't count it */
		c->x86_tlbsize = 0;
#endif
	}

	if (n < 0x80000006)	/* Some chips just has a large L1. */
		return;

	cpuid(0x80000006, &dummy, &ebx, &ecx, &edx);
	l2size = ecx >> 16;

#ifdef CONFIG_X86_64
	c->x86_tlbsize += ((ebx >> 16) & 0xfff) + (ebx & 0xfff);
#else
	/* do processor-specific cache resizing */
	if (this_cpu->c_size_cache)
		l2size = this_cpu->c_size_cache(c, l2size);

	/* Allow user to override all this if necessary. */
	if (cachesize_override != -1)
		l2size = cachesize_override;

	if (l2size == 0)
		return;		/* Again, no L2 cache is possible */
#endif

	c->x86_cache_size = l2size;
}

u16 __read_mostly tlb_lli_4k[NR_INFO];
u16 __read_mostly tlb_lli_2m[NR_INFO];
u16 __read_mostly tlb_lli_4m[NR_INFO];
u16 __read_mostly tlb_lld_4k[NR_INFO];
u16 __read_mostly tlb_lld_2m[NR_INFO];
u16 __read_mostly tlb_lld_4m[NR_INFO];

/*
 * tlb_flushall_shift shows the balance point in replacing cr3 write
 * with multiple 'invlpg'. It will do this replacement when
 *   flush_tlb_lines <= active_lines/2^tlb_flushall_shift.
 * If tlb_flushall_shift is -1, means the replacement will be disabled.
 */
s8  __read_mostly tlb_flushall_shift = -1;

void __cpuinit cpu_detect_tlb(struct cpuinfo_x86 *c)
{
	if (this_cpu->c_detect_tlb)
		this_cpu->c_detect_tlb(c);

	printk(KERN_INFO "Last level iTLB entries: 4KB %d, 2MB %d, 4MB %d\n" \
		"Last level dTLB entries: 4KB %d, 2MB %d, 4MB %d\n"	     \
		"tlb_flushall_shift: %d\n",
		tlb_lli_4k[ENTRIES], tlb_lli_2m[ENTRIES],
		tlb_lli_4m[ENTRIES], tlb_lld_4k[ENTRIES],
		tlb_lld_2m[ENTRIES], tlb_lld_4m[ENTRIES],
		tlb_flushall_shift);
}

void __cpuinit detect_ht(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_HT
	u32 eax, ebx, ecx, edx;
	int index_msb, core_bits;
	static bool printed;

	if (!cpu_has(c, X86_FEATURE_HT))
		return;

	if (cpu_has(c, X86_FEATURE_CMP_LEGACY))
		goto out;

	if (cpu_has(c, X86_FEATURE_XTOPOLOGY))
		return;

	cpuid(1, &eax, &ebx, &ecx, &edx);

	smp_num_siblings = (ebx & 0xff0000) >> 16;

	if (smp_num_siblings == 1) {
		printf_once(HAL_INFO "CPU0: Hyper-Threading is disabled\n");
		goto out;
	}

	if (smp_num_siblings <= 1)
		goto out;

	if (smp_num_siblings > nr_cpu_ids) {
		pr_warning("CPU: Unsupported number of siblings %d",
			smp_num_siblings);
		smp_num_siblings = 1;
		return;
	}

	index_msb = get_count_order(smp_num_siblings);
	c->phys_proc_id = apic->phys_pkg_id(c->initial_apicid, index_msb);

	smp_num_siblings = smp_num_siblings / c->x86_max_cores;

	index_msb = get_count_order(smp_num_siblings);

	core_bits = get_count_order(c->x86_max_cores);

	c->cpu_core_id = apic->phys_pkg_id(c->initial_apicid, index_msb) &
		((1 << core_bits) - 1);

out:
	if (!printed && (c->x86_max_cores * smp_num_siblings) > 1) {
		printf(HAL_INFO  "CPU: Physical Processor ID: %d\n",
			c->phys_proc_id);
		printf(HAL_INFO  "CPU: Processor Core ID: %d\n",
			c->cpu_core_id);
		printed = 1;
	}
#endif
}

static void __cpuinit get_cpu_vendor(struct cpuinfo_x86 *c)
{
	char *v = c->x86_vendor_id;
	int i;

	for (i = 0; i < X86_VENDOR_NUM; i++) {
		if (!cpu_devs[i])
			break;

		if (!strcmp(v, cpu_devs[i]->c_ident[0]) ||
			(cpu_devs[i]->c_ident[1] &&
			!strcmp(v, cpu_devs[i]->c_ident[1]))) {

				this_cpu = cpu_devs[i];
				c->x86_vendor = this_cpu->c_x86_vendor;
				return;
		}
	}

	hal_printf_warning( "CPU: vendor_id '%s' unknown, using generic init.\n" \
		"CPU: Your system may be unstable.\n", v);

	c->x86_vendor = X86_VENDOR_UNKNOWN;
	this_cpu = &default_cpu;
}

void __cpuinit cpu_detect(struct cpuinfo_x86 *c)
{
	/* Get vendor name */
	cpuid(0x00000000, (unsigned int *)&c->cpuid_level,
		(unsigned int *)&c->x86_vendor_id[0],
		(unsigned int *)&c->x86_vendor_id[8],
		(unsigned int *)&c->x86_vendor_id[4]);

	c->x86 = 4;
	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		u32 junk, tfms, cap0, misc;

		cpuid(0x00000001, &tfms, &misc, &junk, &cap0);
		c->x86 = (tfms >> 8) & 0xf;
		c->x86_model = (tfms >> 4) & 0xf;
		c->x86_mask = tfms & 0xf;

		if (c->x86 == 0xf)
			c->x86 += (tfms >> 20) & 0xff;
		if (c->x86 >= 0x6)
			c->x86_model += ((tfms >> 16) & 0xf) << 4;

		if (cap0 & (1<<19)) {
			c->x86_clflush_size = ((misc >> 8) & 0xff) * 8;
			c->x86_cache_alignment = c->x86_clflush_size;
		}
	}
}

void __cpuinit get_cpu_cap(struct cpuinfo_x86 *c)
{
	u32 tfms, xlvl;
	u32 ebx;

	/* Intel-defined flags: level 0x00000001 */
	if (c->cpuid_level >= 0x00000001) {
		u32 capability, excap;

		cpuid(0x00000001, &tfms, &ebx, &excap, &capability);
		c->x86_capability[0] = capability;
		c->x86_capability[4] = excap;
	}

	/* Additional Intel-defined flags: level 0x00000007 */
	if (c->cpuid_level >= 0x00000007) {
		u32 eax, ebx, ecx, edx;

		cpuid_count(0x00000007, 0, &eax, &ebx, &ecx, &edx);

		if (eax > 0)
			c->x86_capability[9] = ebx;
	}

	/* AMD-defined flags: level 0x80000001 */
	xlvl = cpuid_eax(0x80000000);
	c->extended_cpuid_level = xlvl;

	if ((xlvl & 0xffff0000) == 0x80000000) {
		if (xlvl >= 0x80000001) {
			c->x86_capability[1] = cpuid_edx(0x80000001);
			c->x86_capability[6] = cpuid_ecx(0x80000001);
		}
	}

	if (c->extended_cpuid_level >= 0x80000008) {
		u32 eax = cpuid_eax(0x80000008);

		c->x86_virt_bits = (eax >> 8) & 0xff;
		c->x86_phys_bits = eax & 0xff;
	}
#ifdef CONFIG_X86_32
	else if (cpu_has(c, X86_FEATURE_PAE) || cpu_has(c, X86_FEATURE_PSE36))
		c->x86_phys_bits = 36;
#endif

	if (c->extended_cpuid_level >= 0x80000007)
		c->x86_power = cpuid_edx(0x80000007);

	init_scattered_cpuid_features(c);
}

static void __cpuinit identify_cpu_without_cpuid(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_32
	int i;

	/*
	* First of all, decide if this is a 486 or higher
	* It's a 486 if we can modify the AC flag
	*/
	if (flag_is_changeable_p(X86_EFLAGS_AC))
		c->x86 = 4;
	else
		c->x86 = 3;

	for (i = 0; i < X86_VENDOR_NUM; i++)
		if (cpu_devs[i] && cpu_devs[i]->c_identify) {
			c->x86_vendor_id[0] = 0;
			cpu_devs[i]->c_identify(c);
			if (c->x86_vendor_id[0]) {
				get_cpu_vendor(c);
				break;
			}
		}
#endif
}
/*
 * The NOPL instruction is supposed to exist on all CPUs of family >= 6;
 * unfortunately, that's not true in practice because of early VIA
 * chips and (more importantly) broken virtualizers that are not easy
 * to detect. In the latter case it doesn't even *fail* reliably, so
 * probing for it doesn't even work. Disable it completely on 32-bit
 * unless we can find a reliable way to detect all the broken cases.
 * Enable it explicitly on 64-bit for non-constant inputs of cpu_has().
 */
static void __cpuinit detect_nopl(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_32
	clear_cpu_cap(c, X86_FEATURE_NOPL);
#else
	set_cpu_cap(c, X86_FEATURE_NOPL);
#endif
}

static void __cpuinit generic_identify(struct cpuinfo_x86 *c)
{
	c->extended_cpuid_level = 0;

	if (!have_cpuid_p())
		identify_cpu_without_cpuid(c);

	/* cyrix could have cpuid enabled via c_identify()*/
	if (!have_cpuid_p())
		return;

	cpu_detect(c);

	get_cpu_vendor(c);

	get_cpu_cap(c);

	if (c->cpuid_level >= 0x00000001) {
		c->initial_apicid = (cpuid_ebx(1) >> 24) & 0xFF;
#ifdef CONFIG_X86_32
# ifdef CONFIG_X86_HT
		c->apicid = apic->phys_pkg_id(c->initial_apicid, 0);
# else
		c->apicid = c->initial_apicid;
# endif
#endif

#ifdef CONFIG_X86_HT
		c->phys_proc_id = c->initial_apicid;
#endif
	}

	get_model_name(c); /* Default name */

	detect_nopl(c);
}
unsigned long loops_per_jiffy = (1<<12);
/*
 * This does the hard work of actually picking apart the CPU stuff...
 */
static void __cpuinit identify_cpu(struct cpuinfo_x86 *c)
{
	int i;

	c->loops_per_jiffy = loops_per_jiffy;
	c->x86_cache_size = -1;
	c->x86_vendor = X86_VENDOR_UNKNOWN;
	c->x86_model = c->x86_mask = 0;	/* So far unknown... */
	c->x86_vendor_id[0] = '\0'; /* Unset */
	c->x86_model_id[0] = '\0';  /* Unset */
	c->x86_max_cores = 1;
	c->x86_coreid_bits = 0;
#ifdef CONFIG_X86_64
	c->x86_clflush_size = 64;
	c->x86_phys_bits = 36;
	c->x86_virt_bits = 48;
#else
	c->cpuid_level = -1;	/* CPUID not detected */
	c->x86_clflush_size = 32;
	c->x86_phys_bits = 32;
	c->x86_virt_bits = 32;
#endif
	c->x86_cache_alignment = c->x86_clflush_size;
	memset(&c->x86_capability, 0, sizeof c->x86_capability);

	generic_identify(c);

	if (this_cpu->c_identify)
		this_cpu->c_identify(c);

	/* Clear/Set all flags overriden by options, after probe */
	for (i = 0; i < NCAPINTS; i++) {
		c->x86_capability[i] &= ~cpu_caps_cleared[i];
		c->x86_capability[i] |= cpu_caps_set[i];
	}

#ifdef CONFIG_X86_64
	c->apicid = apic->phys_pkg_id(c->initial_apicid, 0);
#endif

	/*
	 * Vendor-specific initialization.  In this section we
	 * canonicalize the feature flags, meaning if there are
	 * features a certain CPU supports which CPUID doesn't
	 * tell us, CPUID claiming incorrect flags, or other bugs,
	 * we handle them here.
	 *
	 * At the end of this section, c->x86_capability better
	 * indicate the features this CPU genuinely supports!
	 */
	if (this_cpu->c_init)
		this_cpu->c_init(c);

	/* Disable the PN if appropriate */
	squash_the_stupid_serial_number(c);

	/* Set up SMEP/SMAP */
	setup_smep(c);
	setup_smap(c);
	
	/*
	 * The vendor-specific functions might have changed features.
	 * Now we do "generic changes."
	 */

	/* Filter out anything that depends on CPUID levels we don't have */
	filter_cpuid_features(c, true);

	/* If the model name is still unset, do table lookup. */
	if (!c->x86_model_id[0]) {
		const char *p;
		p = table_lookup_model(c);
		if (p)
			strcpy(c->x86_model_id, p);
		else
			/* Last resort... */
			sprintf(c->x86_model_id, "%02x/%02x",
				c->x86, c->x86_model);
	}

#ifdef CONFIG_X86_64
	detect_ht(c);
#endif

	init_hypervisor(c);

	/*
	 * Clear/Set all flags overriden by options, need do it
	 * before following smp all cpus cap AND.
	 */
	for (i = 0; i < NCAPINTS; i++) {
		c->x86_capability[i] &= ~cpu_caps_cleared[i];
		c->x86_capability[i] |= cpu_caps_set[i];
	}

	/*
	 * On SMP, boot_cpu_data holds the common feature set between
	 * all CPUs; so make sure that we indicate which features are
	 * common between the CPUs.  The first time this routine gets
	 * executed, c == &boot_cpu_data.
	 */
	if (c != &boot_cpu_data) {
		/* AND the already accumulated flags with these */
		for (i = 0; i < NCAPINTS; i++)
			boot_cpu_data.x86_capability[i] &= c->x86_capability[i];
	}

	/* Init Machine Check Exception if available. */
	mcheck_cpu_init(c);

	select_idle_routine(c);

#ifdef CONFIG_NUMA
	numa_add_cpu(smp_processor_id());
#endif
}

/*
 * Do minimum CPU detection early.
 * Fields really needed: vendor, cpuid_level, family, model, mask,
 * cache alignment.
 * The others are not touched to avoid unwanted side effects.
 *
 * WARNING: this function is only called on the BP.  Don't add code here
 * that is supposed to run on all CPUs.
 */
static void __init early_identify_cpu(struct cpuinfo_x86 *c)
{
#ifdef CONFIG_X86_64
	c->x86_clflush_size = 64;
	c->x86_phys_bits = 36;
	c->x86_virt_bits = 48;
#else
	c->x86_clflush_size = 32;
	c->x86_phys_bits = 32;
	c->x86_virt_bits = 32;
#endif
	c->x86_cache_alignment = c->x86_clflush_size;

	memset(&c->x86_capability, 0, sizeof c->x86_capability);
	c->extended_cpuid_level = 0;

	if (!have_cpuid_p())
		identify_cpu_without_cpuid(c);

	/* cyrix could have cpuid enabled via c_identify()*/
	if (!have_cpuid_p())
		return;

	cpu_detect(c);

	get_cpu_vendor(c);

	get_cpu_cap(c);

	if (this_cpu->c_early_init)
		this_cpu->c_early_init(c);

	c->cpu_index = 0;
	filter_cpuid_features(c, false);

	if (this_cpu->c_bsp_init)
		this_cpu->c_bsp_init(c);
}

void __init identify_boot_cpu(void)
{
}

void __cpuinit identify_secondary_cpu(struct cpuinfo_x86 *c)
{

}
/*
	初始化系统时探测0号CPU，仅仅是early_cpu_init
*/
void __init early_cpu_init()
{
	const struct cpu_dev *const *cdev;
	int count = 0;

	for (cdev = __x86_cpu_dev_start; cdev < __x86_cpu_dev_end; cdev++) {
		const struct cpu_dev *cpudev = *cdev;

		if (count >= X86_VENDOR_NUM)
			break;
		cpu_devs[count] = cpudev;
		count++;
	}

	early_identify_cpu(&boot_cpu_data);

	hal_printf("探测到0号CPU %s-%s.\n",
		boot_cpu_data.x86_vendor_id, boot_cpu_data.x86_model_id);

	if (cpu_has_vme || cpu_has_tsc || cpu_has_de)
		clear_in_cr4(X86_CR4_VME|X86_CR4_PVI|X86_CR4_TSD|X86_CR4_DE);
}

/**
	@brief Cpu advanced init, should run by each cpu
*/
void __cpuinit cpu_init(void)
{
	//TODO: identify_cpu on this cpu 
	fpu_init();
	//xsave_init();
}

