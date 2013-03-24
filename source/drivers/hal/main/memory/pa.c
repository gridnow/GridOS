/**
*   See the readme.txt at the root directory of this project for the idea and originality of this operating system.
*   See the license.txt at the root directory of this project for the copyright information about this file and project.
*
*   Wuxin
*   物理PAGE分配器
*/
#include <debug.h>
#include <string.h>
#include <bitops.h>

#include "memory.h" 
#include "cpu.h"

struct km_stack km_global;
/************************************************************************/
/* 内置的一些节点资源，如果确实太多，可以用分配器来分配                 */
/************************************************************************/
static struct km_cluster_head static_nodes[KM_MAX_STATIC_NODE_INFO];
static struct km_cluster static_clusters[KM_MAX_STATIC_RAM_INFO];
static int cur_ram_info = 0;
static int cur_node_info = 0;

struct km_cluster_show_ctx
{
	struct km_cluster * cluster;
	unsigned long total_pages, free_pages;
};
static struct km_cluster * alloc_cluster()
{
	struct km_cluster *p;
	if (cur_ram_info >= KM_MAX_STATIC_RAM_INFO)
	{
		TRACE_UNIMPLEMENTED("Add code to support big ram info");
		return NULL;
	}
	p = &static_clusters[cur_ram_info++];
	memset(p, 0, sizeof(*p));
	return p;
}

static struct km_cluster_head * alloc_cluster_head(int node)
{
	struct km_cluster_head *p;
	if (cur_node_info >= KM_MAX_STATIC_NODE_INFO)
	{
		//<Add code here> to support big node info 
		return NULL;
	}
	p = &static_nodes[cur_node_info++];
	memset(p, 0, sizeof(*p));
	INIT_LIST_HEAD(&p->clusters);
	ke_spin_init(&p->lock);
	p->node = node;
	return p;
}

/**
	@brief Change the page cluster of current cpu

	@note Must be called at preempt disabled!
 */
static struct km_cluster *set_current_cluster(struct km_cluster *cur)
{
	struct km_cluster *old = kc_get_raw()->mm_current_cluster;
	kc_get_raw()->mm_current_cluster = cur;
	return old;
}

static void show_cluster(struct km_cluster_show_ctx * ctx)
{	
	struct km_cluster * cluster = ctx->cluster;
	printk("簇(%x)总页量:%d, 空闲页数:%d, ",
		cluster,
		cluster->usbale_count, cluster->free);
	printk("实际起始地址:%x, 有效起始地址:%x, ",
		cluster->start_pfn << PAGE_SHIFT, cluster->ram_base);
	printk("终止地址:%x.\n", cluster->end_pfn << PAGE_SHIFT);

	ctx->total_pages += cluster->usbale_count;
	ctx->free_pages  += cluster->free;
}

/* 释放页 */
static void deallocate_page(unsigned long page, unsigned int count)
{
	struct km_cluster_head *node;
	struct km_cluster *cluster;
	unsigned long bit;

	/* Find out which node the page belongs to */
	node = km_get_page_node(page, &cluster);
	if (!node)
	{
		printk("释放页时，没有找到页所对应的NODE，地址是%x，内部错误。\n", page);
		return;
	}

	/* The bit allocator */	
	bit = (page - cluster->ram_base) / PAGE_SIZE;
	if (count == 1)
	{	
		if (cl_bitmap_dealloc_bit(&cluster->bitmap, bit) == false)
		{
			/* Sanity check */
			//printk("释放页时没有释放成功,地址%x, size %d?\n", page, size);
			goto end;
		}
	}
	else
		cl_bitmap_dealloc_consistent_bits(&cluster->bitmap, count, bit);
		
	cluster->free += count; 
	//printk("free %d, p %x.\n", count, page);
end:
	return ;
}

/**
	@brief Allocate a page in preempt disabled mode
*/
static unsigned long allocate(struct km_cluster *cluster, int count)
{
	unsigned long page;

	/* Totally full ? */
	if (cluster->free == 0)
		goto current_full;

	if (count != 1)
		page = cl_bitmap_alloc_consistant_bits(&cluster->bitmap, count);
	else
		page = cl_bitmap_alloc_bit(&cluster->bitmap);
	if (page == HAL_BITMAP_ALLOC_BIT_ERROR)
	{
		page = NULL;
		goto end;
	}
	page = page * PAGE_SIZE/*Offset from the ram*/ + cluster->ram_base;
	cluster->free -= count;

//	printk("Allocate count %d address %x.\n", count, page);

end:
	return page;

current_full:
	return NULL;
}

/* 分配页, 返回物理地址 */
static unsigned long allocate_page(unsigned int count)
{
	struct km_cluster *ram;
	struct list_head *list;
	struct km_cluster_head *head; 
	unsigned long page = NULL;
	bool tried_new_cluster = false, tried_next_node = false;

	/* Case 0, Fast: Current cluster have page? */
	ram = km_get_current_cluster();
	if (unlikely(!ram)) goto end0;

allocate_in_current:
	page = allocate(ram, count);
	if (page) goto end;
	if (tried_new_cluster == true) goto end;

	/* 
		Case 1, Slow: 
		Previously used cluster may have page, if not allocate a new cluster.
	*/
	{
		struct km_cluster * next = NULL;
		struct ke_mem_cluster_info ctx;

		/* If failed again, we do not run the subroutine again */
		tried_new_cluster = true;

		/* 
			Loop the cluster in the list but also at ALLOCATED status(allocated before) to find one with free pages.
			Free cluster should not be touched, because they do not belong to me now!
			And we only check clusters for this cpu.
		*/
		head = ram->head;		
		list_for_each(list, &head->clusters)
		{
			next = list_entry(list, struct km_cluster, nodes);
			if (ram != next /* Not current node */&&
				ram->real_node == next->real_node &&
				test_bit(KM_CLUSTER_NOT_FREE, &next->flags))
			{
				if (next->free >= count)
					break;
			}
			next = NULL;
		}

		/* Found the next cluster on current list, set as current cluster */
		if (next)
		{
			set_current_cluster(next);
			ram = next;
			goto allocate_in_current;
		}

		/* Still not found, then try a new cluster */
		if (km_cluster_alloc(&ctx, 
			kc_get_raw()->id/* We are in preempt disable mode, so RAW version */,
			false/* 一次性不使用完页 */))
		{
			next = ctx.km_cluster;
			set_current_cluster(next);
			ram = next;
			//printk("Allocate page use NEW cluster %x.\n", next);
			goto allocate_in_current;
		}
	}

	/* Case 2, Slowest: Other node may have pages */
	//TODO("当前节点上的内存分配完了，尝试下一个节点,尝试抢占其他CPU上的内存");
	
end:	
	km_put_current_cluster();
end0:
	return page;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
/**
	@brief 分配内核页，并返回内核可直接访问的地址
*/
void *km_page_alloc_kerneled(int count)
{
	unsigned long kp = allocate_page(count);
	if (!kp) return NULL;
	return (void*)HAL_GET_BASIC_KADDRESS(kp);
}

/**
	@brief 释放内核页
*/
void km_page_dealloc_kerneled(void *kernel_page, unsigned long size)
{
	unsigned long page = HAL_GET_BASIC_PHYADDRESS(kernel_page); 
	unsigned int count = KM_PAGE_ROUND_COUNT(size);
	deallocate_page(page, count);
}

/**
	@brief 释放普通页
*/
void km_page_dealloc(unsigned long phy_page, unsigned long size)
{
	unsigned long page = (unsigned long)phy_page;
	unsigned int count = KM_PAGE_ROUND_COUNT(size);
	deallocate_page(page, count);
}

/**
	@brief Alloc a page for user process
*/
unsigned long km_page_alloc()
{
	unsigned long p;
	//TODO allocate the user page not from the head of the bitmap(which is used for kernel)
	p = allocate_page(1);
	if (!p)
	{
		//TODO start recaller
	}

	return p;
}

/**
	@brief Get current cluster of current CPU

	@note Preempt will be disabled
*/
struct km_cluster *km_get_current_cluster()
{
	struct kc_cpu *cpu = kc_get();
	struct km_cluster *cluster = cpu->mm_current_cluster;
	if (cluster) goto end;

	/* Slow mode to get a cluster */
	cluster = km_cluster_alloc(NULL, cpu->id, false);
	if (unlikely(!cluster))
		goto end;
	set_current_cluster(cluster);
end:
	return cluster;
}

void km_put_current_cluster()
{
	kc_put();
}

/**
	@brief Get the node by a page address
*/
struct km_cluster_head *km_get_page_node(unsigned long page, struct km_cluster **back_ram)
{
	struct km_cluster_head * node;
	struct list_head *list;

	list_for_each(list, &km_global.ram_nodes)
	{
		struct list_head *ram_list;
		struct km_cluster *ram;

		node = list_entry(list, struct km_cluster_head, nodes);
		list_for_each(ram_list, &node->clusters)
		{
			ram = list_entry(ram_list, struct km_cluster, nodes);
			if (ram->start_pfn <= KM_PAGE_ADDRESS_TO_PFN(page) &&
				ram->end_pfn >= KM_PAGE_ADDRESS_TO_PFN(page))
			{
				if (back_ram) *back_ram = ram;
				return node;
			}
		}
	}

	return NULL;
}

bool km_insert_ram(unsigned long start, unsigned long size, int node)
{
	struct list_head *tmp;
	struct km_cluster_head *mem_node;
	struct km_cluster *ram;
	unsigned long page_count, bitmap_size, flags, t;
	int i;
	bool r = false;	

	/* Must meet the boundary */
	if (start & (PAGE_SIZE - 1)) goto out;	
	
	/* The km_global lock,  防止km_global被多人修改 */
	flags = ke_spin_lock_irqsave(&km_global.lock);
	
	/* Find the cpu node structure to insert, if no such a node, create one */
	mem_node = NULL;
	list_for_each(tmp, &km_global.ram_nodes)
	{
		mem_node = list_entry(tmp, struct km_cluster_head, nodes);
		if (mem_node->node == node)
			break;
		else
			mem_node = NULL;
	}
	if (mem_node == NULL)
	{ 
		mem_node = alloc_cluster_head(node);
		if (!mem_node) goto insert_end;
		list_add_tail(&mem_node->nodes, &km_global.ram_nodes);
	}

	/* 把这个内存区域分解成一个一个CLUSTER，并找到对应的Cluster链表并插入 */
	i = (size + KM_CLUSTER_SIZE - 1) / KM_CLUSTER_SIZE;
	for (; i > 0; i--)
	{
		/* Current cluster's size */
		t = KM_CLUSTER_SIZE > size ? size : KM_CLUSTER_SIZE;

		/* New cluster */
		ram = alloc_cluster();
		if (!ram) goto insert_end;

		/* Fill the new cluster's information */
		ram->start_pfn		= KM_PAGE_ADDRESS_TO_PFN(start);
		ram->end_pfn		= KM_PAGE_ADDRESS_TO_PFN(start + t - 1);	
		page_count			= ram->end_pfn - ram->start_pfn + 1;									// If start_pfn == end_pfn,there is one page
		bitmap_size			= cl_bitmap_calc_size(page_count);	
		bitmap_size			= KM_PAGE_ROUND_COUNT(bitmap_size);										// 位图本身占用了多少个页？	
		ram->ram_base		= start + bitmap_size * PAGE_SIZE;
		cl_bitmap_init(&ram->bitmap, (unsigned long*)HAL_GET_BASIC_KADDRESS(start), page_count - bitmap_size);
		ram->usbale_count	= page_count - bitmap_size;
		ram->free			= ram->usbale_count;

		/* Insert to the cluster list */
		list_add_tail(&ram->nodes, &mem_node->clusters);
#ifdef __i386__
		printk("Cluster(%x.%d) page count is %d(map page %d), start is %p, end pfn %x.\n", ram, i, page_count, bitmap_size, start, ram->end_pfn);
#endif
		/* We serach next cluster by ram->nodes, no end checking, so gives a bit as the end node in list */
		if (i == 1) __set_bit(KM_CLUSTER_IS_END_NODE, &ram->flags);

		/* Prepare for next cluster */
		start += KM_CLUSTER_SIZE;
		size -= KM_CLUSTER_SIZE;
	}
	r = true;

insert_end:	
	/* Leave critical section */
	ke_spin_unlock_irqrestore(&km_global.lock, flags);
out:
	return r;
}


/**
	@brief Allocate the cluster on the specific cpu
*/
struct km_cluster *km_cluster_alloc(struct ke_mem_cluster_info * ret_info, int node, bool totally_using)
{
	struct km_cluster_head *cluster_head = NULL;
	struct km_cluster * cluster = NULL;
	unsigned long flags;
	struct list_head * tmp;

	/* Select the cluster head by node,and other cpu may adding memory to the cluster list */
	//TODO:to optimize the arithmatic
	flags = ke_spin_lock_irqsave(&km_global.lock);
	list_for_each(tmp, &km_global.ram_nodes)
	{
		cluster_head = list_entry(tmp, struct km_cluster_head, nodes);
		if (cluster_head->node == node)
			break;
		else
			cluster_head = NULL;
	}
	ke_spin_unlock_irqrestore(&km_global.lock, flags);

	/* Found the node? */
	if (cluster_head == NULL)
		goto end;
	
	/* Loop each cluster and find a free one, and other cpu may allocating the memory on our cluster list */
	flags = ke_spin_lock_irqsave(&cluster_head->lock);
	list_for_each(tmp, &cluster_head->clusters)
	{
		cluster = list_entry(tmp, struct km_cluster, nodes);
		
		/* But is already totally using or Is already allocated, we are locked and no atomic ops is needed */
		if (test_bit(KM_CLUSTER_TOTALLY_USING, &cluster->flags) ||
			__test_and_set_bit(KM_CLUSTER_NOT_FREE, &cluster->flags))
		{
			cluster = NULL;
			continue;
		}
		
		cluster->real_node = node;
		cluster->head = cluster_head;
		if (totally_using)
		{
			/* Set totally using which has a heigher priority then NOT_FREE */
			__set_bit(KM_CLUSTER_TOTALLY_USING, &cluster->flags);
			clear_bit(KM_CLUSTER_NOT_FREE, &cluster->flags);
		}
		break;		
	}
	ke_spin_unlock_irqrestore(&cluster_head->lock, flags);

	/* Found the cluster? */
	if (cluster == NULL)
		goto end;

	if (ret_info == NULL)
		goto end;

	/* Tell the user where the memory of the cluster starts */
	ret_info->page_start = cluster->ram_base;
	ret_info->page_count = cluster->usbale_count;
	ret_info->km_cluster = cluster;
end:
	return cluster;
}

/**
	@brief init the memory cluster 
*/
void km_cluster_init()
{
	INIT_LIST_HEAD(&km_global.ram_nodes);
	ke_spin_init(&km_global.lock);
}


