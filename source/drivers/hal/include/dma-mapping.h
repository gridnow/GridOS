#ifndef HAL_DMA_MAPPING_H
#define HAL_DMA_MAPPING_H

#include <ddk/types.h>
#include <ddk/compatible.h>
#include <ddk/dma.h>

struct dma_map_ops {
	dma_addr_t (*map_page)(struct device *dev, struct page *page,
						   unsigned long offset, size_t size,
						   enum dma_data_direction dir,
						   struct dma_attrs *attrs);
	void (*unmap_page)(struct device *dev, dma_addr_t dma_handle,
					   size_t size, enum dma_data_direction dir,
					   struct dma_attrs *attrs);
	int (*map_sg)(struct device *dev, struct scatterlist *sg,
				  int nents, enum dma_data_direction dir,
				  struct dma_attrs *attrs);
	void (*unmap_sg)(struct device *dev,
					 struct scatterlist *sg, int nents,
					 enum dma_data_direction dir,
					 struct dma_attrs *attrs);
	int (*dma_supported)(struct device *dev, u64 mask);

	int is_phys;
};

#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))

#define DMA_MASK_NONE	0x0ULL

#include <asm/dma-mapping.h>
#endif
