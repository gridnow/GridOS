/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   x86 smp support
*/

#ifndef THIS_ARCH_SMP_H
#define THIS_ARCH_SMP_H

//mpparse.c
void mptable_find_smp_config();
void mptable_get_smp_config();


//cpu/boot.c
extern int smp_num_siblings;

//smp/apic.c
extern unsigned int num_processors;

//零时定义的，以后是每个CPU都得有一份
extern u16 x86_bios_cpu_apicid;

//smp ops
struct smp_ops {
	void (*smp_prepare_boot_cpu)(void);
	void (*smp_prepare_cpus)(unsigned max_cpus);
	void (*smp_cpus_done)(unsigned max_cpus);

	void (*stop_other_cpus)(int wait);
	void (*smp_send_reschedule)(int cpu);

	int (*cpu_up)(unsigned cpu);
	int (*cpu_disable)(void);
	void (*cpu_die)(unsigned int cpu);
	void (*play_dead)(void);

	void (*send_call_func_single_ipi)(int cpu);
};
extern struct smp_ops smp_ops;

//boot.c
int native_cpu_up(unsigned int cpunum);

#ifdef CONFIG_SMP
#define startup_ipi_hook(phys_apicid, start_eip, start_esp) do { } while (0)
#endif /* CONFIG_SMP */
#endif /* THIS_ARCH_SMP_H */
