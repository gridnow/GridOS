#include <ddk/dma.h>
#include <ddk/log.h>

struct ddk_dma_pool
{
	void * compatable_pointer;
};

DLLEXPORT void * ddk_dma_pool_alloc(struct ddk_dma_pool * pool, unsigned long flags, dma_addr_t * phy)
{
	TODO("");	
	return NULL;
}

DLLEXPORT void ddk_dma_pool_free(struct ddk_dma_pool *pool, void *vaddr, dma_addr_t dma)
{
	TODO("");
}

DLLEXPORT struct ddk_dma_pool * ddk_dma_pool_create(const char * name, size_t size, size_t align, size_t boundary)
{
	TODO("");
	return NULL;
}

DLLEXPORT void ddk_dma_pool_destroy(struct ddk_dma_pool * pool)
{
	TODO("");
}

DLLEXPORT struct ddk_scatterlist *ddk_sg_next(struct ddk_scatterlist *sg)
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