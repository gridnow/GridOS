
#ifndef ARCH_WALK_H
#define ARCH_WALK_H

#if defined(CONFIG_PAGE_SIZE_16KB) && defined (CONFIG_64BIT)
#define ARCH_KMM_LV1_COUNT 2048
#define ARCH_KMM_LV2_COUNT 2048
#elif defined(CONFIG_PAGE_SIZE_4KB)
#define ARCH_KMM_LV1_COUNT 1024
#define ARCH_KMM_LV2_COUNT 1024
#else
#error "MIPS 平台的KMM 页表项没有确定数量"
#endif

/* Walk */
#define ARCH_KM_TOP_MOST_LEVEL		2

static inline void km_write_sub_table(unsigned long *table, int sub_id, unsigned long phyiscal)
{
	table[sub_id] = phyiscal | PAGE_FLAG_VALID;
}


#define ARCH_HAS_KM_GET_VID
static inline unsigned long km_get_vid(unsigned long level, unsigned long virtual_address)
{
	/* 64 BITS mode */
	if (sizeof(unsigned long) == 8 && PAGE_SIZE == 16384)
	{
		/* (35 25) (24 14) (13 0) */
		switch (level)
		{
		case 2:
			virtual_address >>= 24;
			virtual_address &= (ARCH_KMM_LV2_COUNT - 1);
			break;

		case 1:
			virtual_address >>= 14;
			virtual_address &= (ARCH_KMM_LV1_COUNT - 1);;
			break;

		default:
			virtual_address = -1;
		}
	}
	else if (sizeof(unsigned long) == 4)
	{
		/* (31 22) (21 12) (11 0) */
		switch (level)
		{
		case 2:
			virtual_address >>= 21;
			virtual_address &= (ARCH_KMM_LV2_COUNT - 1);
			break;

		case 1:
			virtual_address >>= 12;
			virtual_address &= (ARCH_KMM_LV1_COUNT - 1);;
			break;

		default:
			virtual_address = -1;
		}
	}
	else
		virtual_address = -1;
	//TODO LV3

	return virtual_address;
}

#endif
