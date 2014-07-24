#include <ddk/dma.h>
#include <ddk/debug.h>

#include "dma-mapping.h"
struct ddk_dma_pool
{
	void * compatable_pointer;
};

void * ddk_dma_pool_alloc(struct ddk_dma_pool *pool, unsigned long flags, dma_addr_t *phy)
{
	TODO("");	
	return NULL;
}

void ddk_dma_pool_free(struct ddk_dma_pool *pool, void *vaddr, dma_addr_t dma)
{
	TODO("");
}

struct ddk_dma_pool * ddk_dma_pool_create(const char * name, size_t size, size_t align, size_t boundary)
{
	TODO("");
	return NULL;
}

void ddk_dma_pool_destroy(struct ddk_dma_pool *pool)
{
	TODO("");
}

/*******************************************************************/
/* scatter list */
/*******************************************************************/
struct scatterlist *sg_next(struct scatterlist *sg)
{
#ifdef CONFIG_DEBUG_SG
	BUG_ON(sg->sg_magic != SG_MAGIC);
#endif
	if (sg_is_last(sg))
		return NULL;

	sg++;
	if (unlikely(sg_is_chain(sg)))
		sg = sg_chain_ptr(sg);

	return sg;
}

/*******************************************************************/
/* DMA Mapping */
/*******************************************************************/
int ddk_dma_set_mask(struct device *dev, u64 mask)
{
	return dma_set_mask(dev, mask);
}

void *ddk_dma_alloc_attrs(struct device *dev, size_t size, dma_addr_t *dma_handle,
						  gfp_t gfp, struct dma_attrs *attrs)
{
TODO("");
}

void ddk_dma_free_attrs(struct device *dev, size_t size,
							  void *vaddr, dma_addr_t bus, struct dma_attrs *attrs)
{
TODO("");
}

dma_addr_t ddk_dma_map_single_attrs(struct device *dev, void *ptr,
											  size_t size,
											  enum dma_data_direction dir,
											  struct dma_attrs *attrs)
{
	struct dma_map_ops *ops = get_dma_ops(dev);
	dma_addr_t addr;
	
	addr = ops->map_page(dev, virt_to_page(ptr),
						 (unsigned long)ptr & ~PAGE_MASK, size,
						 dir, attrs);
	return addr;
}

void ddk_dma_unmap_single_attrs(struct device *dev, dma_addr_t addr,
										  size_t size,
										  enum dma_data_direction dir, struct dma_attrs *attrs)
{
TODO("");
};

