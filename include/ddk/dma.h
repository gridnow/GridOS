/**
*  @defgroup DeviceDMA
*  @ingroup DDK
*
*  定义了设备的DMA内存操作接口
*  @{
*/
#ifndef DDK_DMA_H
#define DDK_DMA_H

#include <ddk/types.h>
#include <ddk/debug.h>
#include <ddk/compatible.h>
#include <ddk/slab.h>

struct ddk_dma_pool;

#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))
#define DMA_MASK_NONE	0x0ULL
enum dma_data_direction {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
};

/*******************************************************************/
/* scatter list */
/*******************************************************************/
#define CONFIG_NEED_SG_DMA_LENGTH 1
struct scatterlist 
{
#ifdef CONFIG_DEBUG_SG
	unsigned long	sg_magic;
#endif
	unsigned long	page_link;
	unsigned int	offset;
	unsigned int	length;
	dma_addr_t	dma_address;
#ifdef CONFIG_NEED_SG_DMA_LENGTH
	unsigned int	dma_length;
#endif
};

#define sg_dma_address(sg)	(((struct scatterlist *)(sg))->dma_address)
#ifdef CONFIG_NEED_SG_DMA_LENGTH
#define sg_dma_len(sg)		(((struct scatterlist *)(sg))->dma_length)
#else
#define sg_dma_len(sg)		((sg)->length)
#endif

struct ddk_sg_table {
	struct scatterlist *sgl;	/* the list */
	unsigned int nents;		/* number of mapped entries */
	unsigned int orig_nents;	/* original size of list */
};
#define SG_MAGIC	0x87654321
#define sg_is_chain(sg)		(((struct scatterlist *)(sg))->page_link & 0x01)
#define sg_is_last(sg)		(((struct scatterlist *)(sg))->page_link & 0x02)
#define sg_chain_ptr(sg)	\
	((struct scatterlist *) (((struct scatterlist *)(sg))->page_link & ~0x03))

/**
 * sg_assign_page - Assign a given page to an SG entry
*/
static inline void sg_assign_page(struct scatterlist *sg, struct page *page)
{
	unsigned long page_link = sg->page_link & 0x3;

	/*
	 * In order for the low bit stealing approach to work, pages
	 * must be aligned at a 32-bit boundary as a minimum.
	 */
	BUG_ON((unsigned long) page & 0x03);
	sg->page_link = page_link | (unsigned long) page;
}

/**
 * sg_set_page - Set sg entry to point at given page
*/
static inline void sg_set_page(struct scatterlist *sg, struct page *page,
							   unsigned int len, unsigned int offset)
{
	sg_assign_page(sg, page);
	sg->offset = offset;
	sg->length = len;
}

static inline struct page *sg_page(struct scatterlist *sg)
{
	return (struct page *)((sg)->page_link & ~0x3);
}

/*
 * Loop over each sg element, following the pointer to a new list if necessary
 */
#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = sg_next(sg))

static inline dma_addr_t sg_phys(struct scatterlist *sg)
{
	return page_to_phys(sg_page(sg)) + sg->offset;
}

DLLEXPORT struct scatterlist *sg_next(struct scatterlist *);

/*******************************************************************/
/* DMA pool */
/*******************************************************************/

/**
	@brief 从缓冲池中分配DMA内存
*/
DLLEXPORT void *ddk_dma_pool_alloc(struct ddk_dma_pool *pool, unsigned long flags, dma_addr_t *phy);

/**
	@brief 释放DMA内存到缓冲池
*/
DLLEXPORT void ddk_dma_pool_free(struct ddk_dma_pool *pool, void *vaddr, dma_addr_t dma);

/**
	@brief 销毁DMA缓冲池
*/
DLLEXPORT void ddk_dma_pool_destroy(struct ddk_dma_pool *pool);

/**
	@brief 创立DMA缓冲池
*/
DLLEXPORT struct ddk_dma_pool * ddk_dma_pool_create(const char *name, size_t size, size_t align, size_t boundary);

/*******************************************************************/
/* DMA Mapping */
/*******************************************************************/
struct dma_attrs;
DLLEXPORT int ddk_dma_set_mask(struct device *dev, u64 mask);
DLLEXPORT void *ddk_dma_alloc_attrs(struct device *dev, size_t size, dma_addr_t *dma_handle,
									gfp_t gfp, struct dma_attrs *attrs);
DLLEXPORT void ddk_dma_free_attrs(struct device *dev, size_t size,
							  void *vaddr, dma_addr_t bus, struct dma_attrs *attrs);

DLLEXPORT dma_addr_t ddk_dma_map_single_attrs(struct device *dev, void *ptr,
								size_t size,
								enum dma_data_direction dir, struct dma_attrs *attrs);

DLLEXPORT void ddk_dma_unmap_single_attrs(struct device *dev, dma_addr_t addr,
										  size_t size,
										  enum dma_data_direction dir, struct dma_attrs *attrs);

static inline void dma_unmap_sg(struct device *dev, struct scatterlist *sg, int nhwentries,
				  enum dma_data_direction direction)
{
	TODO("");
};

static inline int dma_map_sg(struct device *dev, struct scatterlist *sglist, int nents,
							 enum dma_data_direction direction)
{
	TODO("");
};

static inline void * dmam_alloc_coherent(struct device *dev, size_t size,
										 dma_addr_t *dma_handle, gfp_t gfp)
{
	TODO("");
}


#endif /* DDK_DMA_H */

/** @} */
