/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   wuxin
*   固定尺寸的块分配器
*/

#ifndef COMMON_LIB_BLKBUF_H
#define COMMON_LIB_BLKBUF_H

struct cl_bkb;
typedef void (*cl_bkb_free_handler)(void *para, void *base, size_t size);

/**
	@brief Alloc a node in a block buffer
	
	@param[in] bkb the address of the block buffer descriptor
	@return 
		The address of a block. return NUll on failure
*/
void *cl_bkb_alloc(struct cl_bkb *bkb);

/**
	@brief Deallocate a block from the block buffer

	@param[in] bkb the address of the block buffer descriptor
	@param[in] node the address of the block
*/
void cl_bkb_dealloc(struct cl_bkb *bkb, void *node);

/**
	@brief 动态扩展block buffer
*/
void cl_bkb_extend(struct cl_bkb *bkb, void *base, size_t size, cl_bkb_free_handler handler, void *para);

struct cl_bkb
{	
	xstring name;	
	int		node_size;
	void	*prefer;
};

#endif /* */
