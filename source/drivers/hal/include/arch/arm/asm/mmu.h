#ifndef __ASM_MMU_H
#define __ASM_MMU_H

struct machine_desc;

/* HEAD.S */
void arch_enable_mmu(unsigned long page_table);

/* MMU.C */
void __init early_paging_init();
void __init paging_init();

#endif

