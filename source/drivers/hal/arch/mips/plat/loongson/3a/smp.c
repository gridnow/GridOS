
#include <types.h>
#include <hal_debug.h>
#include <hal_irqflags.h>
#include <linkage.h>

#include <boot_param.h>
#include <asm/mipsregs.h>
#include <asm/ptrace.h>
#include <asm/smp.h>

#include "smp.h"
/**
	@brief the smp base default to 3A
*/
unsigned long smp_group0 = 0x900000003ff01000;
unsigned long smp_group1 = 0x900010003ff01000;
unsigned long smp_group2 = 0x900020003ff01000;
unsigned long smp_group3 = 0x900030003ff01000;

/**
	@brief cpu count default to 4, this is the default value of one 3a cpu
*/
unsigned int nr_cpu_loongson = 4;

static void *mailbox_set0_regs[16];								// addr for core_set0 reg,
static void *mailbox_clear0_regs[16];                           // addr for core_set0 reg,
static void *mailbox_regs0[16];                                 // addr for core_set0 reg,
static void *mailbox_en0_regs[16];	                            // addr for core_set0 reg,
static void *mailbox_buf[16];		                            // addr for core_set0 reg,

static inline void loongson3_raw_writeq(unsigned long action, void * addr)
{                                                          
	/* The SMP register is 32bits but the BUF of startup is 64bits */
	if(((long)addr & 0xff)<0x20)
		*((volatile u32 *)addr) = (u32)action;
	else
		*((volatile unsigned long *)addr) = action;
}

static inline unsigned long loongson3_raw_readq(void * addr) 
{                                                           
	if (((long)addr & 0xff) < 0x20)
		return *((volatile u32*)addr);
	return *((volatile unsigned long*)addr);
}

static void mailbox_set0_regs_init (void){                          // addr for core_set0 reg,
	mailbox_set0_regs[0] = (void *)core0_SET0;               // which is a 32 bit reg
	mailbox_set0_regs[1] = (void *)core1_SET0;               // When the bit of core_set0 is 1,
	mailbox_set0_regs[2] = (void *)core2_SET0;               // the bit of core_status0 become 1
	mailbox_set0_regs[3] = (void *)core3_SET0;               // immediately
	mailbox_set0_regs[4] = (void *)core4_SET0;               // which is a 32 bit reg
	mailbox_set0_regs[5] = (void *)core5_SET0;               // When the bit of core_set0 is 1,
	mailbox_set0_regs[6] = (void *)core6_SET0;               // the bit of core_status0 become 1
	mailbox_set0_regs[7] = (void *)core7_SET0;               // immediately 
	mailbox_set0_regs[8] = (void *)core8_SET0;               // which is a 32 bit reg
	mailbox_set0_regs[9] = (void *)core9_SET0;               // When the bit of core_set0 is 1
	mailbox_set0_regs[10] = (void *)coreA_SET0;              // the bit of core_status0 become 1
	mailbox_set0_regs[11] = (void *)coreB_SET0;              // immediately
	mailbox_set0_regs[12] = (void *)coreC_SET0;              // which is a 32 bit reg
	mailbox_set0_regs[13] = (void *)coreD_SET0;              // When the bit of core_set0 is 1
	mailbox_set0_regs[14] = (void *)coreE_SET0;              // the bit of core_status0 become 1
	mailbox_set0_regs[15] = (void *)coreF_SET0;              // immediately
}

static void *mailbox_clear0_regs_init (void){                           // addr for core_clear0 reg,
	mailbox_clear0_regs[0] = (void *)core0_CLEAR0;               // which is a 32 bit reg 
	mailbox_clear0_regs[1] = (void *)core1_CLEAR0;               // When the bit of core_clear0 is 1,
	mailbox_clear0_regs[2] = (void *)core2_CLEAR0;               // the bit of core_status0 become 0
	mailbox_clear0_regs[3] = (void *)core3_CLEAR0;               // immediately
	mailbox_clear0_regs[4] = (void *)core4_CLEAR0;               // which is a 32 bit reg
	mailbox_clear0_regs[5] = (void *)core5_CLEAR0;               // When the bit of core_clear0 is 1,
	mailbox_clear0_regs[6] = (void *)core6_CLEAR0;               // the bit of core_status0 become 0
	mailbox_clear0_regs[7] = (void *)core7_CLEAR0;               // immediately
	mailbox_clear0_regs[8] = (void *)core8_CLEAR0;               // which is a 32 bit reg
	mailbox_clear0_regs[9] = (void *)core9_CLEAR0;               // When the bit of core_clear0 is 1,
	mailbox_clear0_regs[10] = (void *)coreA_CLEAR0;              // the bit of core_status0 become 0
	mailbox_clear0_regs[11] = (void *)coreB_CLEAR0;              // immediately
	mailbox_clear0_regs[12] = (void *)coreC_CLEAR0;              // which is a 32 bit reg
	mailbox_clear0_regs[13] = (void *)coreD_CLEAR0;              // When the bit of core_clear0 is 1,
	mailbox_clear0_regs[14] = (void *)coreE_CLEAR0;              // the bit of core_status0 become 0
	mailbox_clear0_regs[15] = (void *)coreF_CLEAR0;              //immediately
}

static void *mailbox_regs0_init (void) {                                // addr for core_status0 reg
	mailbox_regs0[0] = (void *)core0_STATUS0;                    // which is a 32 bit reg  
	mailbox_regs0[1] = (void *)core1_STATUS0;                    // the reg is read only
	mailbox_regs0[2] = (void *)core2_STATUS0;            
	mailbox_regs0[3] = (void *)core3_STATUS0;            
	mailbox_regs0[4] = (void *)core4_STATUS0;                    // which is a 32 bit reg
	mailbox_regs0[5] = (void *)core5_STATUS0;                    // the reg is read only
	mailbox_regs0[6] = (void *)core6_STATUS0;            
	mailbox_regs0[7] = (void *)core7_STATUS0;            
	mailbox_regs0[8] = (void *)core8_STATUS0;                    // which is a 32 bit reg
	mailbox_regs0[9] = (void *)core9_STATUS0;                    // the reg is read only
	mailbox_regs0[10] = (void *)coreA_STATUS0;          
	mailbox_regs0[11] = (void *)coreB_STATUS0;          
	mailbox_regs0[12] = (void *)coreC_STATUS0;                   // which is a 32 bit reg
	mailbox_regs0[13] = (void *)coreD_STATUS0;                   // the reg is read only
	mailbox_regs0[14] = (void *)coreE_STATUS0;           
	mailbox_regs0[15] = (void *)coreF_STATUS0;           
}

static void * mailbox_en0_regs_init (void) {                            // addr for core_set0 reg,
	mailbox_en0_regs[0] = (void *)core0_EN0;                    // which is a 32 bit reg
	mailbox_en0_regs[1]	= (void *)core1_EN0;                    // When the bit of core_set0 is 1,
	mailbox_en0_regs[2] = (void *)core2_EN0;                    // the bit of core_status0 become 1
	mailbox_en0_regs[3] = (void *)core3_EN0;                    // immediately                       
	mailbox_en0_regs[4] = (void *)core4_EN0;                    // which is a 32 bit reg
	mailbox_en0_regs[5] = (void *)core5_EN0;                    // When the bit of core_set0 is 1,
	mailbox_en0_regs[6] = (void *)core6_EN0;                    // the bit of core_status0 become 1
	mailbox_en0_regs[7] = (void *)core7_EN0;                    // immediately                       
	mailbox_en0_regs[8] = (void *)core8_EN0;                    // which is a 32 bit reg
	mailbox_en0_regs[9] = (void *)core9_EN0;                    // When the bit of core_set0 is 1,
	mailbox_en0_regs[10] = (void *)coreA_EN0;                   // the bit of core_status0 become 1
	mailbox_en0_regs[11] = (void *)coreB_EN0;                   // immediately                       
	mailbox_en0_regs[12] = (void *)coreC_EN0;                   // which is a 32 bit reg
	mailbox_en0_regs[13] = (void *)coreD_EN0;                   // When the bit of core_set0 is 1,
	mailbox_en0_regs[14] = (void *)coreE_EN0;                   // the bit of core_status0 become 1
	mailbox_en0_regs[15] = (void *)coreF_EN0;                   // immediately                       
}


volatile void *mailbox_buf_init (void)  {                       // addr for core_buf regs
	mailbox_buf[0] = (void *)core0_BUF;                         // a group of regs with 0x40 byte size
	mailbox_buf[1] = (void *)core1_BUF;                         // which could be used for  
	mailbox_buf[2] = (void *)core2_BUF;                         // transfer args , r/w , uncached
	mailbox_buf[3] = (void *)core3_BUF;
	mailbox_buf[4] = (void *)core4_BUF;                         // a group of regs with 0x40 byte size
	mailbox_buf[5] = (void *)core5_BUF;                         // which could be used for  
	mailbox_buf[6] = (void *)core6_BUF;                         // transfer args , r/w , uncached
	mailbox_buf[7] = (void *)core7_BUF;
	mailbox_buf[8] = (void *)core8_BUF;                         // a group of regs with 0x40 byte size
	mailbox_buf[9] = (void *)core9_BUF;                         // which could be used for  
	mailbox_buf[10] = (void *)coreA_BUF;                        // transfer args , r/w , uncached
	mailbox_buf[11] = (void *)coreB_BUF;
	mailbox_buf[12] = (void *)coreC_BUF;                        // a group of regs with 0x40 byte size
	mailbox_buf[13] = (void *)coreD_BUF;                        // which could be used for  
	mailbox_buf[14] = (void *)coreE_BUF;                        // transfer args , r/w , uncached
	mailbox_buf[15] = (void *)coreF_BUF;
}

static int loongson3_cpu_start(int cpu, void(*fn)(void), long sp, long gp, long a1)
{
	int res;
	volatile unsigned long long startargs[4];

	startargs[0] = (long)fn;
	startargs[1] = sp;
	startargs[2] = gp;
	startargs[3] = a1;

	loongson3_raw_writeq(startargs[3], mailbox_buf[cpu] + 0x18);
	loongson3_raw_writeq(startargs[2], mailbox_buf[cpu] + 0x10);
	loongson3_raw_writeq(startargs[1], mailbox_buf[cpu] + 0x8);
	loongson3_raw_writeq(startargs[0], mailbox_buf[cpu] + 0x0);

	res = 0;

	return res;
}

/**
	@brief Send startup signal
	
	Setup the PC, SP, and GP of a secondary processor and start it running, this is defined by the PMON
*/
static void loongson3_boot_secondary(int cpu, void *idle_thread)
{
	int retval;
	int volatile active = smp_get_active_count();
#define TEMP_STACK_SIZE 16384
	static unsigned char temp_statck[4][TEMP_STACK_SIZE] = {1};
	unsigned long sp = (unsigned long)&temp_statck[cpu][TEMP_STACK_SIZE], gp = 0, a1 = cpu;
	
	retval = loongson3_cpu_start(cpu, &smp_bootstrap, sp, gp, a1);

	/* Wait some times */	
	while (active == smp_get_active_count())
	{
		//TODO: add timeout
	}
}

/**
	@brief The secondary CPUs platform initialization
*/
static void loongson3_init_secondary(void)
{
	int i;
	unsigned int imask = /*STATUSF_IP7*/0 | STATUSF_IP6 | STATUSF_IP5 | 
		STATUSF_IP4 | STATUSF_IP3 | STATUSF_IP2 ;

	/* Set interrupt mask, but don't enable */
	change_c0_status(ST0_IM, imask);

	for (i = 0; i < nr_cpu_loongson; i++) {
		loongson3_raw_writeq(0xffffffff, mailbox_en0_regs[i]);
	}
}

/**
	@brief The secondary CPUs finished initialization, prepare to load work
*/
static void loongson3_finish_secondary(void)
{
	write_c0_count(1);
	write_c0_compare(0);
	local_irq_enable();
}

static void loongson3_send_ipi_single(int cpu, unsigned int action)
{
	loongson3_raw_writeq((((u64)action)), mailbox_set0_regs[cpu]);
}

/**
	@brief Setup the smp
*/
static void __init loongson3_smp_setup(void)
{
	mailbox_set0_regs_init();
	mailbox_en0_regs_init();
	mailbox_regs0_init();
	mailbox_clear0_regs_init();
	mailbox_buf_init();
}

/**
	@brief 处理器间中断
*/
void loongson3_ipi_interrupt()
{
	//int cpu = smp_processor_id();
	int cpu = read_c0_ebase() & 0x3ff;//TEMP
	unsigned int action;
	printk("IPI 的CPU编号获取是零时的.\n");

	/* Load the mailbox register to figure out what we're supposed to do */
	action = loongson3_raw_readq(mailbox_regs0[cpu]);	

	/* Clear the mailbox to clear the interrupt */
	loongson3_raw_writeq(action, mailbox_clear0_regs[cpu]);

}

struct plat_smp_ops loongson3_smp_ops = {
	.send_ipi_single	= loongson3_send_ipi_single,
	.init_secondary		= loongson3_init_secondary,
	.finish_secondary	= loongson3_finish_secondary,
	.boot_secondary		= loongson3_boot_secondary,
	.smp_setup			= loongson3_smp_setup,
};
