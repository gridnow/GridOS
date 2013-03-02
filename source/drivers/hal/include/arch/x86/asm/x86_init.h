#ifndef _ASM_X86_PLATFORM_H
#define _ASM_X86_PLATFORM_H

struct mpc_bus;
struct mpc_cpu;
struct mpc_table;
struct cpuinfo_x86;

struct x86_init_mpparse {
	void (*mpc_record)(unsigned int mode);
	void (*setup_ioapic_ids)(void);
	int (*mpc_apic_id)(struct mpc_cpu *m);
	void (*smp_read_mpc_oem)(struct mpc_table *mpc);
	void (*mpc_oem_pci_bus)(struct mpc_bus *m);
	void (*mpc_oem_bus_info)(struct mpc_bus *m, char *name);
	void (*find_smp_config)(void);
	void (*get_smp_config)(unsigned int early);
};

struct x86_init_resources {
	void (*probe_roms)(void);
	void (*reserve_resources)(void);
	char *(*memory_setup)(void);
};

struct x86_init_irqs {
	void (*pre_vector_init)(void);
	void (*intr_init)(void);
	void (*trap_init)(void);
};

struct x86_init_oem {
	void (*arch_setup)(void);
	void (*banner)(void);
};

struct x86_init_timers {
	void (*setup_percpu_clockev)(void);
	void (*tsc_pre_init)(void);
	void (*timer_init)(void);
	void (*wallclock_init)(void);
};

struct x86_init_iommu {
	int (*iommu_init)(void);
};

struct x86_init_ops {
	struct x86_init_resources	resources;
	struct x86_init_mpparse		mpparse;
	struct x86_init_irqs		irqs;
	struct x86_init_oem		oem;
	struct x86_init_timers		timers;
	struct x86_init_iommu		iommu;
};

struct x86_platform_ops {
	unsigned long (*calibrate_tsc)(void);
	unsigned long (*get_wallclock)(void);
	int (*set_wallclock)(unsigned long nowtime);
	void (*iommu_shutdown)(void);
	bool (*is_untracked_pat_range)(u64 start, u64 end);
	void (*nmi_init)(void);
	unsigned char (*get_nmi_reason)(void);
	int (*i8042_detect)(void);
	void (*save_sched_clock_state)(void);
	void (*restore_sched_clock_state)(void);
	void (*apic_post_init)(void);
};

struct x86_io_apic_ops {
	void		(*init)  (void);
	unsigned int	(*read)  (unsigned int apic, unsigned int reg);
	void		(*write) (unsigned int apic, unsigned int reg, unsigned int value);
	void		(*modify)(unsigned int apic, unsigned int reg, unsigned int value);
};

extern struct x86_init_ops x86_init;
extern struct x86_platform_ops x86_platform;
extern struct x86_io_apic_ops x86_io_apic_ops;
extern void x86_init_noop(void);
extern void x86_init_uint_noop(unsigned int unused);

#endif

