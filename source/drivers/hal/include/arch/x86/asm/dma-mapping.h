#ifndef _ASM_X86_DMA_MAPPING_H
#define _ASM_X86_DMA_MAPPING_H

#define DMA_ERROR_CODE	0

extern struct dma_map_ops *dma_ops;

static inline struct dma_map_ops *get_dma_ops(struct device *dev)
{
#ifndef CONFIG_X86_DEV_DMA_OPS
	return dma_ops;
#else
	if (unlikely(!dev) || !dev->archdata.dma_ops)
		return dma_ops;
	else
		return dev->archdata.dma_ops;
#endif
}

extern int dma_set_mask(struct device *dev, u64 mask);
extern int dma_supported(struct device *hwdev, u64 mask);

#ifdef CONFIG_X86_DMA_REMAP /* Platform code defines bridge-specific code */
extern bool dma_capable(struct device *dev, dma_addr_t addr, size_t size);
extern dma_addr_t phys_to_dma(struct device *dev, phys_addr_t paddr);
extern phys_addr_t dma_to_phys(struct device *dev, dma_addr_t daddr);
#else

static inline bool dma_capable(struct device *dev, dma_addr_t addr, size_t size)
{
	if (!dev->dma_mask)
		return 0;

	return addr + size - 1 <= *dev->dma_mask;
}

static inline dma_addr_t phys_to_dma(struct device *dev, phys_addr_t paddr)
{
	return paddr;
}

static inline phys_addr_t dma_to_phys(struct device *dev, dma_addr_t daddr)
{
	return daddr;
}
#endif /* CONFIG_X86_DMA_REMAP */

#endif
