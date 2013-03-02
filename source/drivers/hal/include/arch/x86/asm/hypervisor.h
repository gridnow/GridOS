
#ifndef _ASM_X86_HYPERVISOR_H
#define _ASM_X86_HYPERVISOR_H


extern void init_hypervisor(struct cpuinfo_x86 *c);
extern void init_hypervisor_platform(void);

/*
 * x86 hypervisor information
 */
struct hypervisor_x86 {
	/* Hypervisor name */
	const char	*name;

	/* Detection routine */
	bool		(*detect)(void);

	/* Adjust CPU feature bits (run once per CPU) */
	void		(*set_cpu_features)(struct cpuinfo_x86 *);

	/* Platform setup (run once per boot) */
	void		(*init_platform)(void);
};

extern const struct hypervisor_x86 *x86_hyper;

/* Recognized hypervisors */
extern const struct hypervisor_x86 x86_hyper_vmware;
extern const struct hypervisor_x86 x86_hyper_ms_hyperv;
extern const struct hypervisor_x86 x86_hyper_xen_hvm;


#endif
