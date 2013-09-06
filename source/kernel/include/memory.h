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

#include <spinlock.h>
#include "bitmap.h"

struct km;

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

/* Should be provided by arch */
void km_arch_init_for_kernel(struct km *mem);
void km_arch_ctx_init(struct km * mm_ctx);
void km_arch_ctx_deinit(struct km * mm_ctx);
void km_arch_ctx_switch(struct km * pre_ctx, struct km * next_ctx);

/**
	@brief
*/
bool km_insert_ram(unsigned long start, unsigned long size, int node);

/**
	@brief 分配一个完整的簇
 
	一般分配一个完整的簇是为了每个CPU能并发分配簇里头的内存页面，不过也可以用作为驱动分配一个大的连续的物理内存
 
	@param[in,out] ret_info 回写簇的信息，NULL则表示不回写
	@param[in] node 表示要在哪个节点上分配簇，NUMA/CPU核心与内存的距离，一个优化参数，－1表示随意
	@param[in] totally_using 表示该簇一开始里头的页面是全部被使用的，一般为驱动设置为true，驱动组件自己在里面进行内存分配
 
	@return
		NULL失败，成功则为cluster对象
*/
struct km_cluster *km_cluster_alloc(struct ke_mem_cluster_info *ret_info, int node, bool totally_using);

/**
	@brief 释放一个簇
 
	@param[in] physical_base 通过该物理地址查询簇对应的簇
	@param[in] force 是否强制释放该簇，不论簇里面的内存是否被占用，调用者保证这点，否则系统可能出错，一般在做处理处理时设置为true
*/
void km_cluster_dealloc_base(unsigned long physical_base, bool force);

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

/**************************************************************
 pa.c
**************************************************************/
/**
	@brief Get current cluster of current cpu

	@note:
		This function will disable preempt
*/
struct km_cluster *km_get_current_cluster();

/**
	@brief Allocate a physical page normally for VM map
*/
unsigned long km_page_alloc();

/**
	@brief
*/
void km_page_dealloc(unsigned long phy_page);

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

