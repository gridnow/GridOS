/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   内存管理
*/

#ifndef KM_H
#define KM_H

#include <list.h>

#include <lock.h>
#include "bitmap.h"

typedef unsigned long pte_t;
struct km
{
};

struct km_walk_ctx
{
};

/* Cluster of page */
#define KM_CLUSTER_SIZE				(32 * 1024 * 1024)
#define KM_MAX_STATIC_RAM_INFO		32
#define KM_MAX_STATIC_NODE_INFO		2
#define KM_CLUSTER_NOT_FREE			1
#define KM_CLUSTER_IS_END_NODE		2
#define KM_CLUSTER_TOTALLY_USING	3										// 完全被占用，HAL要用一个完整的CLUSTER，这种CLUSTER不可参与常规的PAGE分配

/* 每个CPU 寻址的物理地址*/
struct km_cluster_head
{
	struct list_head	nodes;	
	struct list_head	clusters;
	struct ke_spinlock	lock;
	int 				node;
};

/* 物理内存簇，每个CPU都有 */
struct km_cluster
{
	struct list_head	nodes;
	struct km_cluster_head *head;
	unsigned long		start_pfn;
	unsigned long		end_pfn;
	unsigned long		ram_base;											//The ram head is the bitmap,so start_pfn >>PAGE_SHIT + BITMAP = ram_base;
	struct cl_bitmap	bitmap;
	int					real_node;

	/* 计数器 */
	unsigned long		usbale_count;
	unsigned long		free;

	/* 状态 */
	unsigned long		flags;
};

/* Kernel global memory info, 所有的CPU地址*/
struct km_stack
{
	struct list_head	ram_nodes;											//Numa memory node list
	struct ke_spinlock	lock;												//Only used for modifying the nodes list or inserting ram
	/* <Add code here for swap,etc.> */
};
extern struct km_stack km_global;

//pa.c
struct ke_mem_cluster_info
{
	unsigned long page_start;
	unsigned long page_count;
	void *km_cluster;
};

void km_cluster_init();

/**
	@brief
*/
bool km_insert_ram(unsigned long start, unsigned long size, int node);

/**
	@brief
*/
struct km_cluster *km_cluster_alloc(struct ke_mem_cluster_info *ret_info, int node, bool totally_using);

/**
	@brief Get the node by a page address

	Find the node by a a page address is used when we are free a page.

	@param[in] page The physical address
	@param[in,out] back_ram If !NULL, write back the ram node for the page address.

	@return The node object or NULL if error.
*/
struct km_cluster_head *km_get_page_node(unsigned long page, struct km_cluster **back_ram);

/**
	@brief
*/
void km_put_current_cluster();

/**
	@brief Get current cluster of current cpu

	@note:
		This function will disable preempt
*/
struct km_cluster *km_get_current_cluster();

/**
	@brief
*/
unsigned long km_page_alloc();

/**
	@brief
*/
void km_page_dealloc(unsigned long phy_page, unsigned long size);

/**
	@brief
*/
void *km_page_alloc_kerneled(int page_count);

/**
	@brief
*/
void km_page_dealloc_kerneled(void *kernel_page, unsigned long size);

/* Useful macro */
#define KM_PAGE_ADDRESS_TO_PFN(PAGE_ADDRESS) ((unsigned long)(PAGE_ADDRESS) >> PAGE_SHIFT)
#define KM_PAGE_ROUND_COUNT(__size__) ((__size__ + PAGE_SIZE - 1) / PAGE_SIZE)
#define KM_PAGE_ROUND_ALIGN(__address__) ((unsigned long)(__address__) & ~ (PAGE_SIZE - 1))
#define KM_PAGE_GET_OFFSET(__address__) ((unsigned long)(__address__) & (PAGE_SIZE - 1))

#endif

