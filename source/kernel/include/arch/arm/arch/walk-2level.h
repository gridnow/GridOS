/**
 *   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
 *   See the license.txt at the root directory of this project for the copyright information about this file and project.
 *
 *   Wuxin(82828068@qq.com)
 *   ARM 32-bit 2¼¶×ª»»±í
 */
#ifndef ARM_WALK_2level_H
#define ARM_WALK_2level_H

#define ARCH_KM_LV1_COUNT	512
#define ARCH_KM_LV2_COUNT	2048
#define KM_WALK_MAX_LEVEL	2

/* In level2, we use specific get table mode */
#define ARCH_HAS_GET_SUB_TABLE
static inline void *km_get_sub_table(unsigned long *table, int sub_id)
{
	unsigned long sub_table_phy;
	lv2_entry_t *lv2_table = (lv2_entry_t*)table;
	
	sub_table_phy = KM_PAGE_PHY_FROM_ENTRY(lv2_table[sub_id].odd, sub_id);
	if (sub_table_phy == NULL)
		return NULL;
	
	//TODO: to support non-identical mapping tables
	return (void*)HAL_GET_BASIC_KADDRESS(sub_table_phy);
}

static inline void km_write_sub_table(unsigned long *table, int sub_id, unsigned long phyiscal)
{
	unsigned long prot = _PAGE_KERNEL_TABLE;
	lv2_entry_t *lv2_table = (lv2_entry_t*)table;
	
	phyiscal = (phyiscal + PTE_HWTABLE_OFF) | prot;
	
	lv2_table[sub_id].odd  = phyiscal;
	lv2_table[sub_id].even = phyiscal + 256 * sizeof(pte_t) ;
	flush_pmd_entry(&table[sub_id]);
}

#endif
