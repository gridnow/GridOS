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
#include <ddk/log.h>

struct ddk_dma_pool;
#define DMA_BIT_MASK(n)	(((n) == 64) ? ~0ULL : ((1ULL<<(n))-1))
#define DMA_MASK_NONE	0x0ULL
#define CONFIG_NEED_SG_DMA_LENGTH 1

enum dma_data_direction {
	DMA_BIDIRECTIONAL = 0,
	DMA_TO_DEVICE = 1,
	DMA_FROM_DEVICE = 2,
	DMA_NONE = 3,
};

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
#define SG_MAGIC	0x87654321
#define sg_is_chain(sg)		(((struct scatterlist *)(sg))->page_link & 0x01)
#define sg_is_last(sg)		(((struct scatterlist *)(sg))->page_link & 0x02)
#define sg_chain_ptr(sg)	\
	((struct scatterlist *) (((struct scatterlist *)(sg))->page_link & ~0x03))

struct ddk_sg_table {
	struct scatterlist *sgl;	/* the list */
	unsigned int nents;		/* number of mapped entries */
	unsigned int orig_nents;	/* original size of list */
};

/**
	@brief 获取下一个sg
*/
struct scatterlist *ddk_sg_next(struct scatterlist *);

static inline void sg_init_one(struct scatterlist * a, const void *b, unsigned int c)
{
	TODO("");
}

/*
 * Loop over each sg element, following the pointer to a new list if necessary
 */
#define for_each_sg(sglist, sg, nr, __i)	\
	for (__i = 0, sg = (sglist); __i < (nr); __i++, sg = ddk_sg_next(sg))

/**
	@brief 从缓冲池中分配DMA内存
*/
void * ddk_dma_pool_alloc(struct ddk_dma_pool * pool, unsigned long flags, dma_addr_t * phy);

/**
	@brief 释放DMA内存到缓冲池
*/
void ddk_dma_pool_free(struct ddk_dma_pool *pool, void *vaddr, dma_addr_t dma);

/**
	@brief 销毁DMA缓冲池
*/
void ddk_dma_pool_destroy(struct ddk_dma_pool * pool);

/**
	@brief 创立DMA缓冲池
*/
struct ddk_dma_pool * ddk_dma_pool_create(const char * name, size_t size, size_t align, size_t boundary);

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
