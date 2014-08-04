
#include <loongson.h>
#include <compiler.h>
#include <types.h>
#include <hal_print.h>
#include <hal_debug.h>
#include <hal_bitops.h>

#include <private/interrupt.h>
#include <private/irq.h>

#include <asm/irq.h>
#include <asm/irq_cpu.h>
#include <asm/i8259.h>
#include <ASM/MIPSREGs.h>

#include "htregs.h"
#include "irqregs.h"

// /*
//  * This contains the irq mask for both 8259A irq controllers,
//  */
// static unsigned int cached_irq_mask = 0xffff;
// static struct ke_spinlock ht_lock;
// static inline void ht_irq_disable(struct irq_data *d)
// {
// 	unsigned int mask;
// 	unsigned long flags;
// 	int irq = d->irq;
// 
// 	mask = 1 << (irq - I8259A_IRQ_BASE);
// 	flags = ke_spin_lock_irqsave(&ht_lock);
// 	cached_irq_mask |= mask;
// 
// 	HT_irq_enable_reg0 = ~(0xffff0000|cached_irq_mask);
// 	mmiowb();
// 
// 	ke_spin_unlock_irqrestore(&ht_lock, flags);
// }
// 
// static inline void ht_irq_enable(struct irq_data *d)
// {
// 	unsigned int mask;
// 	unsigned long flags;
// 	int irq = d->irq;printk("Enable ht_irq %d\n", irq);
// 
// 	mask = ~(1 << (irq - I8259A_IRQ_BASE));
// 	flags = ke_spin_lock_irqsave(&ht_lock);
// 	cached_irq_mask &= mask;
// 
// 	HT_irq_enable_reg0 = ~(0xffff0000|cached_irq_mask);
// 	mmiowb();
// 
// 	ke_spin_unlock_irqrestore(&ht_lock, flags);
// }
// 
// 
// static struct irq_chip irq_type = {
// 	.name	= "ÁúÐ¾3 HTÖÐ¶Ï¿ØÖÆÆ÷",
// 	.irq_ack	= ht_irq_disable,
// 	.irq_mask	= ht_irq_disable,
// 	.irq_mask_ack = ht_irq_disable,
// 	.irq_unmask	= ht_irq_enable,
// };

void __init ht_irq_init(void)
{
	unsigned int t;
	u32 i;

	/*
	* Clear all of the interrupts while we change the able around a bit.
	* int-handler is not on bootstrap
	*/
	clear_c0_status(ST0_IM | ST0_BEV);
	local_irq_disable();

	/* Route the LPC interrupt to Core0 INT0 */
	INT_router_regs_lpc_int = 0x11;
	*(volatile unsigned int *)(0x900000003ff00000 + 0x1428) = (0x1<<10);//Enable lpc interrupts

	/* Route the HT interrupt to Core0 INT1 */
	INT_router_regs_HT1_int0 = 0x21;
	INT_router_regs_HT1_int1 = 0x21;
	INT_router_regs_HT1_int2 = 0x21;
	INT_router_regs_HT1_int3 = 0x21;
	INT_router_regs_HT1_int4 = 0x21;
	INT_router_regs_HT1_int5 = 0x21;
	INT_router_regs_HT1_int6 = 0x21;
	INT_router_regs_HT1_int7 = 0x21;

	HT_irq_enable_reg0 = 0xffffffff;
	HT_irq_enable_reg1 = 0x00000000;
	HT_irq_enable_reg2 = 0x00000000;
	HT_irq_enable_reg3 = 0x00000000;
	HT_irq_enable_reg4 = 0x00000000;
	HT_irq_enable_reg5 = 0x00000000;
	HT_irq_enable_reg6 = 0x00000000;
	HT_irq_enable_reg7 = 0x00000000;

	/* Enable the IO interrupt controller */
	t = IO_control_regs_Intenset;
	IO_control_regs_Intenset = t | (0xffff << 16);
	IO_control_regs_Intenset = t | (0xffff << 16) | (0x1 << 10);
	t = IO_control_regs_Intenset;

//  	for (i = I8259A_IRQ_BASE; i < I8259A_IRQ_BASE + 16; i++)
//  		irq_set_chip_and_handler(i, &irq_type, handle_level_irq);
//  	ke_spin_init(&ht_lock);

	init_i8259_irqs();
}