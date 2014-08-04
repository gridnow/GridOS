#ifndef __HTREGS__H__
#define __HTREGS__H__

extern unsigned long loongson3_ht_control_base;
#define HT_control_regs_base loongson3_ht_control_base

#define HT_irq_vector_reg0	*(volatile unsigned int *)(HT_control_regs_base + 0x80)	
#define HT_irq_vector_reg1	*(volatile unsigned int *)(HT_control_regs_base + 0x84)	
#define HT_irq_vector_reg2	*(volatile unsigned int *)(HT_control_regs_base + 0x88)	
#define HT_irq_vector_reg3	*(volatile unsigned int *)(HT_control_regs_base + 0x8C)	
#define HT_irq_vector_reg4	*(volatile unsigned int *)(HT_control_regs_base + 0x90)	
#define HT_irq_vector_reg5	*(volatile unsigned int *)(HT_control_regs_base + 0x94)	
#define HT_irq_vector_reg6	*(volatile unsigned int *)(HT_control_regs_base + 0x98)	
#define HT_irq_vector_reg7	*(volatile unsigned int *)(HT_control_regs_base + 0x9C)	

#define HT_irq_enable_reg0	*(volatile unsigned int *)(HT_control_regs_base + 0xA0)	
#define HT_irq_enable_reg1	*(volatile unsigned int *)(HT_control_regs_base + 0xA4)	
#define HT_irq_enable_reg2	*(volatile unsigned int *)(HT_control_regs_base + 0xA8)	
#define HT_irq_enable_reg3	*(volatile unsigned int *)(HT_control_regs_base + 0xAC)	
#define HT_irq_enable_reg4	*(volatile unsigned int *)(HT_control_regs_base + 0xB0)	
#define HT_irq_enable_reg5	*(volatile unsigned int *)(HT_control_regs_base + 0xB4)	
#define HT_irq_enable_reg6	*(volatile unsigned int *)(HT_control_regs_base + 0xB8)	
#define HT_irq_enable_reg7	*(volatile unsigned int *)(HT_control_regs_base + 0xBC)	

#define HT_uncache_enable_reg0	*(volatile unsigned int *)(HT_control_regs_base + 0xF0)
#define HT_uncache_base_reg0	*(volatile unsigned int *)(HT_control_regs_base + 0xF4)
#define HT_uncache_enable_reg1	*(volatile unsigned int *)(HT_control_regs_base + 0xF8)
#define HT_uncache_base_reg1	*(volatile unsigned int *)(HT_control_regs_base + 0xFC)

#endif
