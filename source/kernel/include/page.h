#ifndef PAGE_H
#define PAGE_H

#include <kernel/kernel.h>
struct km;
bool km_page_map_range(struct km *mem_dst, unsigned long start_va, unsigned long size, unsigned long physical_pfn, page_prot_t prot);
bool km_page_create(struct km *mem_dst, unsigned long address, page_prot_t prot);

#endif
