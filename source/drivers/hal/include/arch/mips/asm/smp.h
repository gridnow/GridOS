#ifndef _ASM_SMP_H
#define _ASM_SMP_H

/* head.S to startup secondary cpu */
extern void smp_bootstrap(void);

/**
	@brief MIPS SMP OPS
*/
struct plat_smp_ops 
{
	/* 
		@brief The system is going to send an IPI to the destination CPU
	*/
	void (*send_ipi_single)(int cpu, unsigned int action);

	/**
		@brief The non-BSP CPU is preparing to init itself
	*/
	void (*init_secondary)(void);

	/**
		@brief The non-BSP CPU is preparing to finish initialization
	*/
	void (*finish_secondary)(void);

	/**
		@brief Boot the specified cpu in the SMP architecture
	*/
	void (*boot_secondary)(int cpu, void *idle_thread);

	/**
		@brief The init interface for the SMP module on the platform, called when the SMP module operation method is registered to system
	*/
	void (*smp_setup)(void);
};

/* smp.c */
extern struct plat_smp_ops *mp_ops;
void register_smp_ops(struct plat_smp_ops *ops);
int smp_boot_aps();
void smp_dec_active_count();
void smp_add_active_count();
int smp_get_active_count();

/* The arch has the SMP init module */
extern void arch_init_smp();
#endif
