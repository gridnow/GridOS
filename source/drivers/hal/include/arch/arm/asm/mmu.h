#ifndef __ASM_MMU_H
#define __ASM_MMU_H

struct machine_desc;

/* HEAD.S */
void arch_enable_mmu(unsigned long page_table);

/* MMU.C */
const struct mem_type *get_mem_type(unsigned int type);
void __init early_paging_init();
void __init paging_init();
void *hal_arm_get_init_pgtable();

#endif

