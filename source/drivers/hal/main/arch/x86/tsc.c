#include <types.h>
#include <asm-generic/param.h>

#include <asm/cpufeature.h>
#include <asm/x86_init.h>

unsigned int __read_mostly cpu_khz;	/* TSC clocks / usec, not used here */
unsigned int __read_mostly tsc_khz;

/*
 * TSC can be unstable due to cpufreq or due to unsynced TSCs
 */
static int __read_mostly tsc_unstable;

/* native_sched_clock() is called before tsc_init(), so
   we must start with the TSC soft disabled to prevent
   erroneous rdtsc usage on !cpu_has_tsc processors */
static int __read_mostly tsc_disabled = -1;

int tsc_clocksource_reliable;

static void __init check_system_tsc_reliable(void)
{
#ifdef CONFIG_MGEODE_LX
	/* RTSC counts during suspend */
#define RTSC_SUSP 0x100
	unsigned long res_low, res_high;

	rdmsr_safe(MSR_GEODE_BUSCONT_CONF0, &res_low, &res_high);
	/* Geode_LX - the OLPC CPU has a very reliable TSC */
	if (res_low & RTSC_SUSP)
		tsc_clocksource_reliable = 1;
#endif
	if (boot_cpu_has(X86_FEATURE_TSC_RELIABLE))
		tsc_clocksource_reliable = 1;
}

void mark_tsc_unstable(char *reason)
{
	
}

void __init tsc_init(void)
{
	u64 lpj;
	int cpu;

	x86_init.timers.tsc_pre_init();

	if (!cpu_has_tsc)
		return;

	tsc_khz = x86_platform.calibrate_tsc();
	cpu_khz = tsc_khz;

	if (!tsc_khz) {
		mark_tsc_unstable("could not calculate TSC khz");
		return;
	}
#if 0
	pr_info("Detected %lu.%03lu MHz processor\n",
		(unsigned long)cpu_khz / 1000,
		(unsigned long)cpu_khz % 1000);

	/*
	 * Secondary CPUs do not run through tsc_init(), so set up
	 * all the scale factors for all CPUs, assuming the same
	 * speed as the bootup CPU. (cpufreq notifiers will fix this
	 * up if their speed diverges)
	 */
	for_each_possible_cpu(cpu)
		set_cyc2ns_scale(cpu_khz, cpu);

	if (tsc_disabled > 0)
		return;

	/* now allow native_sched_clock() to use rdtsc */
	tsc_disabled = 0;

	if (!no_sched_irq_time)
		enable_sched_clock_irqtime();

	lpj = ((u64)tsc_khz * 1000);
	do_div(lpj, HZ);
	lpj_fine = lpj;

	use_tsc_delay();

	if (unsynchronized_tsc())
		mark_tsc_unstable("TSCs unsynchronized");

	check_system_tsc_reliable();
#endif	
}

