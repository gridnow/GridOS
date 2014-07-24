#ifndef __ASM_MACH_MAP_H
#define __ASM_MACH_MAP_H

/* MT_DEVICE etc is put to dma_mapping */
#include <asm/dma-mapping.h>

int arm_bsp_create_map(struct map_desc *map_info, int size);
#endif
